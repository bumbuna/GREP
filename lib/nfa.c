//Bumbuna <developer@devbumbuna.com>
//2022
#include "include/nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//default lexical analyzer
extern int getchar_wrapper(char *store) {
    while((*store = getchar()) != EOF) {
        int escaped = 0;
        if(*store == '\\') {
            escaped = 1;
            *store = getchar();
        }
        return escaped ? LITERAL : ascii2TOKEN[*store];
    }
    return END_OF_REGEX;
}

static char(*regex_characters)() = getchar_wrapper;
static int current_token;
static char current_char;
#define match(x) (current_token == x)
#define advance() (current_token = (*regex_characters)(&current_char))
#define match_closure() (match(TIMES)||match(PLUS)||match(QUESTION))
#define MAX_NFA_STATES 256
static struct nfa *nfa_block;
static struct nfa *nfa_block_ptr;
static int free_states[33];
#define push_to_freestates(x) do {\
                                    free_states[++free_states[0]] = x; \
                                    if(free_states[0]>=33) {\
                                        nfa_error_handler(ERR_COMPLEX_REGEX); \
                                    } \
                                } while(0)
#define pop_from_freestates()   free_states[free_states[0]--]
#define no_freestates() !(free_states[0])

static struct nfa *rule(void);
static void regex(struct nfa **start, struct nfa **end);
static void cat(struct nfa **start, struct nfa **end);
static void closured(struct nfa **start, struct nfa **end);
static void unit(struct nfa **start, struct nfa **end);
static void fill_label_ccl(struct set *s);
static struct nfa *nfa_new(void);
static void nfa_recycle(struct nfa *n);
static int can_concat(void);

//int,... -> void
//interface to the nfa module routines
//cmd is the action to perform
//... cmd specific arguments 
extern void *nfa_terminal(int cmd, ...) {
    va_list l;
    va_start(l, cmd);
    switch(cmd) {
        case CMD_MAKE: {
            regex_characters = va_arg(l, void *);
            if(!regex_characters) {
                regex_characters = getchar_wrapper;
            }
            struct nfa **start = va_arg(l, struct nfa **);
            advance();
            *start = rule();
            break;
        }
        case CMD_FREE: {
            if(nfa_block) {
                free(nfa_block);
                nfa_block = NULL;
            }
            break;
        }
        case CMD_STATE2NUM: {
            struct nfa *n = va_arg(l, void *);
            int *store = va_arg(l, int *);
            *store = (int)(n-nfa_block);
            break;
        }
        case CMD_NUM2STATE: {
            int i = va_arg(l, int);
            struct nfa **store = va_arg(l, void *);
            *store = &nfa_block[i];
            break;
        }
    }
}

//void -> struct nfa *
//PRODUCTION: rule ::= regex END_OF_REGEX 
//return the start state of the nfa
static struct nfa *rule(void) {
    struct nfa *start, *end;
    if(match(BOL)) {
        start = nfa_new();
        start->label = '\n';
        advance();
        regex(&start->next, &end);
    } else {
        regex(&start, &end);
    }
    if(match(EOL)) {
        advance();
        end->next = nfa_new();
        end->label = '\n';
        end = end->next;
        advance();
    }
    if(!match(END_OF_REGEX)) {
        nfa_error_handler(ERR_UNEXPECTED_TOKEN);
    }
    return start;
}

//struct nfa **, struct nfa ** -> void
//PRODUCTION: regex ::= cat opt_or
static void regex(struct nfa **start, struct nfa **end) {
    cat(start, end);
    while(match(OR)) {
        advance();
        struct nfa *start2, *end2;
        cat(&start2, &end2);
        struct nfa *temp = nfa_new();
        temp->next = *start;
        temp->next2 = start2;
        *start = temp;
        temp = nfa_new();
        (*end)->next = temp;
        end2->next = temp;
        *end = temp;
    }
}

//struct nfa **, struct nfa ** -> void
//PRODUCTION: cat ::= closured cat
static void cat(struct nfa **start, struct nfa **end) {
    if(can_concat()) {
        closured(start, end);
    }
    while(can_concat()) {
        struct nfa *start2, *end2;
        closured(&start2, &end2);
        memmove(*end, start2, sizeof(struct nfa));
        nfa_recycle(start2);
    }
}

//struct nfa **, struct nfa ** -> void
//PRODUCTION: closured ::= unit opt_closure
static void closured(struct nfa **start, struct nfa **end) {
    unit(start, end);
    if(match_closure()) {
        struct nfa *new_start = nfa_new();
        struct nfa *new_end = nfa_new();
        new_start->next = *start;
        (*end)->next = new_end;
        if(match(TIMES)||match(PLUS)) {
            (*end)->next2 = *start;
        }
        if(match(TIMES)||match(QUESTION)) {
            new_start->next2 = new_end;
        }
        *start = new_start;
        *end = new_end;
        advance();
    }
}

//struct nfa **, struct nfa ** -> void
//PRODUCTION: unit ::= literal | ( regex )| [ stuff ]| .
static void unit(struct nfa **start, struct nfa **end) {
    if(match(PAREN_OPEN)) {
        advance();
        regex(start, end);
        if(!match(PAREN_CLOSE)) {
            nfa_error_handler(ERR_MISSING_CLOSING_PAREN);
        }
    } else if(match(LITERAL)||match(ANY)||match(CCL_OPEN)) {
        *start = nfa_new();
        (*start)->next = *end = nfa_new();
        if(!match(ANY)&&!match(CCL_OPEN)) {
            (*start)->label = current_char;
        } else {
            struct set *s;
            (*start)->label = LABEL_CCL;
            s = (*start)->label_class = set_create();
            if(match(ANY)) {
                set_add(s, '\n');
                set_toggle_complement(s);
            } else {
                advance();
                fill_label_ccl(s);
                if(!match(CCL_CLOSE)) {
                    nfa_error_handler(ERR_MISSING_CLOSING_SQUARE_BRACKET);
                }
                advance();
            }
        }
    } else {
        nfa_error_handler(ERR_UNEXPECTED_TOKEN);
    }
    advance();
}

//struct set * -> void
//add members of a regex's character class to set s
static void fill_label_ccl(struct set *s) {
    int prev = -1;
    if(match(BOL)) {
        advance();
        set_toggle_complement(s);
    }
    while(!match(CCL_CLOSE)&&!match(END_OF_REGEX)) {
        if(match(DASH)) {
            advance();
            if(prev > current_char) {
                prev ^= current_char;
                current_char ^= prev;
                prev ^= current_char;
            }
            for(int i = prev; i <= current_char; i++) {
                set_add(s, i);
            }
        } else {
            set_add(s, current_char);
        }
        prev = current_char;
        advance();
    }
}

//void -> struct nfa *
//allocate and intialize memory for a new nfa state
static struct nfa *nfa_new(void) {
    if(!nfa_block) {
        if(!(nfa_block = calloc(MAX_NFA_STATES, sizeof(struct nfa)))) {
            nfa_error_handler(ERR_OUT_OF_MEMORY);
        }
        nfa_block_ptr = nfa_block;
    }
    struct nfa *n;
    if(no_freestates()) {
        n = nfa_block_ptr++;
        if(nfa_block_ptr >= &nfa_block[MAX_NFA_STATES]) nfa_error_handler(ERR_TOO_MANY_STATES);
    } else {
        n = &nfa_block[pop_from_freestates()];
    }
    n->label = LABEL_EPSILON;
    return n;
}

//struct nfa * -> void
//clear and recycle state n's memory
static void nfa_recycle(struct nfa *n) {
    push_to_freestates(n-nfa_block);
    memset(n, 0, sizeof(struct nfa));
}

//handle error_code raised internally by the nfa module
//default implementation prints error info and terminates program
extern void nfa_error_handler(int error_code) {
    fprintf(stderr, "NFA: %s, offending token `%c`\n",
            error_code_to_string[error_code], current_char);
    exit(1);
}

//void -> int
//check if current_token is concatable
static int can_concat(void) {
    switch(current_token) {
        case TIMES:
        case PLUS:
        case QUESTION: nfa_error_handler(ERR_UNBOUND_CLOSURE);
        // case PAREN_CLOSE:nfa_error_handler(ERR_UNEXPECTED_CLOSING_PAREN);
        case CCL_CLOSE: nfa_error_handler(ERR_UNEXPECTED_CLOSING_SQUARE_BRACKET);
        case BOL: nfa_error_handler(ERR_UNEXPECTED_BOL);
        case END_OF_REGEX:
        case EOL:
        case PAREN_CLOSE: return 0;
        default: return 1;
    }
}

char *error_code_to_string[] = {
    "",
    "ERR_UNEXPECTED_CHARACTER",
    "ERR_MISSING_CLOSING_PAREN",
    "ERR_MISSING_CLOSING_SQUARE_BRACKET",
    "ERR_UNEXPECTED_BOL",
    "ERR_TOO_MANY_STATES",
    "ERR_OUT_OF_MEMORY",
    "ERR_COMPLEX_REGEX",
    "ERR_UNBOUND_CLOSURE",
    "ERR_UNEXPECTED_CLOSING_PAREN",
    "ERR_UNEXPECTED_CLOSING_SQUARE_BRACKET",
};

enum TOKENS ascii2TOKEN[] = {  
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    END_OF_REGEX,LITERAL,LITERAL,LITERAL,EOL,LITERAL,LITERAL,LITERAL,
    PAREN_OPEN,PAREN_CLOSE,TIMES,LITERAL,LITERAL,DASH,ANY,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,QUESTION,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,CCL_OPEN,LITERAL,CCL_CLOSE,BOL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
    LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,LITERAL,
};
