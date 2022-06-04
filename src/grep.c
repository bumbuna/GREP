//Bumbuna <developer@devbumbuna.com>
//2022
#include "include/nfa.h"
#include "include/bitset.h"
#include "include/stack.h"
#include <stdio.h>
#include <string.h>

static int dispatch_newline = 1;
static long line_offset;
static FILE *file;
#define BUFFER_SIZE 128
static char read_buffer[BUFFER_SIZE];
static int read_buffer_i = BUFFER_SIZE;
static int read_buffer_sz = BUFFER_SIZE;
static int eof_read = 0;
static int line_no = 0;
static int dispatch_eof = 0;
static char *argv1;

//void -> int
//get next character from <b>file</b> stream
static int get_next_character() {
    if(dispatch_eof) return -1;
    if(dispatch_newline) {
        dispatch_newline = 0;
        // line_offset = ftell(file)-(read_buffer_sz-read_buffer_i);
        ++line_no;
        return '\n';
    }
    if(read_buffer_i >= read_buffer_sz) {
        if(eof_read)  {
            dispatch_eof = 1;
            return '\n';
        }
        read_buffer_sz = fread_unlocked(read_buffer, sizeof(char),
                                         BUFFER_SIZE, file);
        if(read_buffer_sz != BUFFER_SIZE) {
            eof_read = 1;
        }
        read_buffer_i = 0;
    }
    if(read_buffer[read_buffer_i] == '\n') {
        // dispatch_newline = 1;
        line_offset = ftell(file)-(read_buffer_sz-read_buffer_i)+1;
        // ++line_no;
    }
    return read_buffer[read_buffer_i++];
}

//void -> int
//print characters from file starting at line_offset until newline is found
//return the line number
static int print_line() {
    fseek(file, line_offset, SEEK_SET);
    while(fgets(read_buffer, BUFFER_SIZE, file)) {
        printf("%s", read_buffer);
        if(read_buffer[strlen(read_buffer)-1] == '\n') {
            ungetc('\n', file);
            break;
        }
    }
    // dispatch_newline = 1;
    read_buffer_i = read_buffer_sz = BUFFER_SIZE;
    return line_no;
}

//struct set * -> struct set *
//perform e-closure on set s
static struct set *e_closure(struct set *s) {
    stack_create(states, 128, int);
    set_nextmember(NULL);
    int m;
    while((m = set_nextmember(s)) != -1) {
        stack_push(states, m);
    }
    while(!stack_isempty(states)) {
        struct nfa *n;
        nfa_terminal(CMD_NUM2STATE, stack_pop(states), &n);
        if(n->label == LABEL_EPSILON) {
            int i;
            if(n->next) {
                nfa_terminal(CMD_STATE2NUM, &i);
                if(!set_member(s, i)) {
                    set_add(s, i);
                    stack_push(states, i);
                }
            }
            if(n->next2) {
                nfa_terminal(CMD_STATE2NUM, &i);
                if(!set_member(s, i)) {
                    set_add(s, i);
                    stack_push(states, i);
                }
            }
        }
    }
    return s;
}

//int(*)(int*) -> void
//perform regex matching on lines in <b>file</b> 
//function f is passed as an argument for CMD_MAKE in nfa_terminal
static void grep(int (*f)(int*)) {
    struct nfa *state_machine;
    nfa_terminal(CMD_MAKE, f, &state_machine);
    struct set *start_set = set_create();
    int start_set_num;
    nfa_terminal(CMD_STATE2NUM, state_machine, &start_set_num);
    set_add(start_set, start_set_num);
    e_closure(start_set);
    struct set *current_set = set_duplicate(start_set);
    struct set *next_set = set_create();
    int is_accepting_set = 0;
    int c;
    while((c = get_next_character()) != -1) {
        if(!set_size(current_set)) {
            set_assign(current_set, start_set);
        } else {
            int x;
            set_nextmember(NULL);
            while((x = set_nextmember(current_set)) != -1) {
                struct nfa *s;
                nfa_terminal(CMD_NUM2STATE, x, &s);
                if(!s->next && !s->next2) {
                    is_accepting_set = 1;
                    break;
                }
            }
            if(is_accepting_set) {
                is_accepting_set = 0;
                print_line();
                set_clear(current_set);
                continue;
            }
        }
        set_clear(next_set);
        int m;
        set_nextmember(NULL);
        while((m = set_nextmember(current_set)) != -1) {
            struct nfa *s;
            nfa_terminal(CMD_NUM2STATE, m, &s);
            if(s->label == c ||(s->label==LABEL_CCL && set_member(s->label_class, c))) {
                nfa_terminal(CMD_STATE2NUM, s->next, &m);
                set_add(next_set, m);
            }
        }
        set_assign(current_set, e_closure(next_set));
    }
    nfa_terminal(CMD_FREE);
}

//void -> int
//return next character of string given as program's first argument
int argv1_next_char() {
    while(*argv1) {
        return *argv1++;
    }
    return EOF;
} 

//int, char *[] -> int
//launch
int main(int argc, char **argv) {
    argv1 = argv[1];
    file = fopen(argv[2], "r");
    grep(argv1_next_char);
}
