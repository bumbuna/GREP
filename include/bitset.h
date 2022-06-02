//Bumbuna <developer@devbumbuna.com>
//2022
#ifndef BITSET_H
#define BITSET_H
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define BITS_IN_BYTE 8
#define WORD sizeof(int)
#define WORD_BITS (BITS_IN_BYTE * WORD)
#define DEFAULT_SET_B_ARRAY_SZ (256/WORD_BITS)

struct set {
    int b_array_sz;
    int b_array_bits;
    int complement;
    uint *b_array;
    uint def_b_array[DEFAULT_SET_B_ARRAY_SZ];
};

//struct set *, int -> void
//add i to set s
extern void set_add(struct set *s, int i);
//void -> struct set *
//allocate memory for a new set
extern struct set *set_create();
//struct set *, int -> int
//test for i in set s
extern int set_test(struct set *s, int i);
//struct set *s, int -> void
//remove i from set s
extern void set_remove(struct set *s, int i);
//struct set *s
//calculate the size of set s
extern int set_size(struct set *s);
//struct set *, int -> int
//check membership of i in set s
extern int set_member(struct set *s, int i);
//struct set * -> void
//toggle set s' complement flag 
extern void set_toggle_complement(struct set *s);
//struct set * -> void
//invert set s
extern void set_invert(struct set *s);
//struct set * -> void
//clear set s
extern void set_clear(struct set *s);
//struct set *, struct set * -> void
//perform a union of set s and t
//s hold the union
extern void set_union(struct set *s, struct set *t);
//struct set *, struct set * -> void
//perform a difference of set s and t
//s hold the difference
extern void set_difference(struct set *s, struct set *t);
//struct set *, struct set * -> void
//perform an intersect of set s and t
//s hold the intersect
extern void set_intersect(struct set *s, struct set *t);
//struct set * -> struct set *
//duplicate set s
extern struct set *set_duplicate(struct set *s);
//struct set * -> void
//free set s' memory
extern void set_free(struct set *s);
//struct set *, struct set * -> void
//assign members of set t to set s
extern void set_assign(struct set *s, struct set *t);
//struct set *, struct set * -> int
//compare sets s and t
//for s>t return +ve, s<t return -ve, s==t return 0
extern int set_compare(struct set *s, struct set *t);
//struct set *s, void(*)(int) -> void
//apply function f on each member of set s
extern void set_apply(struct set *s, void (*f)(int));
//struct set *s -> int
//get next member of set s, -1 if set is depleted
extern int set_nextmember(struct set *s);
#endif //BITSET_H
