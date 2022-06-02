//Bumbuna <developer@devbumbuna.com>
//2022
#ifndef NFA_H
#define NFA_H
#include "include/bitset.h"

#define LABEL_EMPTY 0
#define LABEL_EPSILON -1
#define LABEL_CCL -2

enum ERROR_CODES {
    ERR_NONE,
    ERR_UNEXPECTED_TOKEN = 1,
    ERR_MISSING_CLOSING_PAREN,
    ERR_MISSING_CLOSING_SQUARE_BRACKET,
    ERR_UNEXPECTED_BOL,
    ERR_TOO_MANY_STATES,
    ERR_OUT_OF_MEMORY,
    ERR_COMPLEX_REGEX,
    ERR_UNBOUND_CLOSURE,
    ERR_UNEXPECTED_CLOSING_PAREN,
    ERR_UNEXPECTED_CLOSING_SQUARE_BRACKET,
};

enum TOKENS {
    BOL,
    EOL,
    ANY,
    TIMES,
    PLUS,
    QUESTION,
    CCL_OPEN,
    CCL_CLOSE,
    PAREN_OPEN,
    PAREN_CLOSE,
    OR,
    DASH,
    LITERAL,
    END_OF_REGEX,
};

#define CMD_MAKE 1
#define CMD_FREE 2
#define CMD_STATE2NUM 3
#define CMD_NUM2STATE 4

//error code as strings
extern char *error_code_to_string[];
//tokens indexed by ascii characters
extern enum TOKENS ascii2TOKEN[];

struct nfa {
    int label;
    struct set *label_class;
    struct nfa *next;
    struct nfa *next2;
};

// //char(*)() -> struct nfa *
// //interface to the nfa making routines
// //f is a function than returns the characters making up the regex
// struct nfa *nfa_interface(char (*f)());


//int,... -> void
//interface to the nfa module routines
//cmd is the action to perform
//... cmd specific arguments 
extern void *nfa_terminal(int cmd, ...);
//int -> void
//handle error_code raised internally by the nfa module
//default implementation prints error info and terminates program
extern void nfa_error_handler(int error_code);

#endif //NFA_H
