#include "include/nfa.h"
#include <stdio.h>

int main() {
    for(int i = 0; i < 128; i++) {
        if(i && !(i%8)) fputc('\n', stdout), fflush(stdout);
        switch(i) {
            case '.': fputs("ANY", stdout); break;
            case '?': fputs("QUESTION", stdout); break;
            case '*': fputs("TIMES", stdout); break;
            case '-': fputs("DASH", stdout); break;
            case '[': fputs("CCL_OPEN", stdout); break;
            case ']': fputs("CCL_CLOSE", stdout); break;
            case '(': fputs("PAREN_OPEN", stdout); break;
            case ')': fputs("PAREN_CLOSE", stdout); break;
            case '^': fputs("BOL", stdout); break;
            case '$': fputs("EOL", stdout); break;
            case ' ': fputs("END_OF_REGEX", stdout); break;
            default: fputs("LITERAL", stdout);
        }
        fputc(',', stdout);
    }
}

