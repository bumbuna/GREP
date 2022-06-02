//Bumbuna <deveolper@devbumbuna.com>
//2022

#include "include/nfa.h"

int main(int argc, char **argv) {
    struct nfa *state_0;
    nfa_terminal(CMD_MAKE, NULL, &state_0);
    nfa_terminal(CMD_FREE);
    return 0;
}
