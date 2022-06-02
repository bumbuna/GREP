//Bumbuna <developer@devbumbuna.com>
//2022
#include "include/bitset.h"
#include <string.h>

//unsigned int -> int
//count number of bits set to 1 in c
static int onesies(unsigned int c) {
    int bit_count = 0;
    while(c) {
        if(c&1U) {
            bit_count++;
        }
        c >>= 1;
    }
    return bit_count;
}  

//void -> struct set *
//allocate memory for a new set
extern struct set *set_create() {
    struct set *s = calloc(1, sizeof(struct set));
    if(!s) perror("calloc"), exit(errno);
    s->b_array = s->def_b_array;
    s->b_array_sz = DEFAULT_SET_B_ARRAY_SZ;
    s->b_array_bits = DEFAULT_SET_B_ARRAY_SZ * WORD_BITS;
    return s;
}

//struct set *, int -> void
//add i to set s
extern void set_add(struct set *s, int i) {
    if(i < (s->b_array_bits)) {
        s->b_array[i/WORD_BITS] |= (1U<<i%WORD_BITS);
    } else {
        int new_b_array_sz = (i/WORD_BITS)+1;
        int *new_b_array = calloc(new_b_array_sz, WORD);
        for(int i = 0; i < s->b_array_sz; i++) {
            new_b_array[i] = s->b_array[i];
        }
        if(s->b_array != s->def_b_array) {
            free(s->b_array);
        }
        s->b_array = new_b_array;
        s->b_array_sz = new_b_array_sz;
        s->b_array_bits = new_b_array_sz * WORD_BITS;
        set_add(s, i);
    }
}

//struct set *, int -> int
//test for i in set s
extern int set_test(struct set *s, int i) {
    if(i >= s->b_array_bits) {
        return 0;
    }
    return s->b_array[i/WORD_BITS] & (1U<<i%WORD_BITS);
}

//struct set *s, int -> void
//remove i from set s
extern void set_remove(struct set *s, int i) {
    if(i < s->b_array_bits) {
        s->b_array[i/WORD_BITS] &= ~(1U<<i%WORD_BITS);
    }
}

//struct set *s
//calculate the size of set s
extern int set_size(struct set *s) {
    unsigned char *b_array_as_uchar = (typeof(b_array_as_uchar)) s->b_array;
    int b_array_byte_count = s->b_array_sz * WORD;
    int sz = 0;
    for(int i = 0; i < b_array_byte_count; i++) {
        sz += onesies(b_array_as_uchar[i]);
    }
    return sz;
}

//struct set *, int -> int
//check membership of i in set s
extern int set_member(struct set *s, int i) {
    return s->complement ? !set_test(s, i) : set_test(s, i);
}

//struct set * -> void
//toggle set s' complement flag 
extern void set_toggle_complement(struct set *s) {
    s->complement = !s->complement;
}

//struct set * -> void
//invert set s
extern void set_invert(struct set *s) {
    for(int i = 0; i < s->b_array_sz; i++) {
        s->b_array[i] = ~s->b_array[i];
    }
}

//struct set * -> void
//clear set s
extern void set_clear(struct set *s) {
    for(int i = 0; i < s->b_array_sz; i++) {
        s->b_array[i] = 0U;
    }
}

//struct set *, struct set * -> void
//perform a union of set s and t
//s hold the union
extern void set_union(struct set *s, struct set *t) {
    if(!set_test(s, t->b_array_bits-1)) {
        set_add(s, t->b_array_bits-1);
        set_remove(s, t->b_array_bits-1);
    }
    for(int i = 0; i < t->b_array_sz; i++) {
        s->b_array[i] |= t->b_array[i];
    }
}

//struct set *, struct set * -> void
//perform a difference of set s and t
//s hold the difference
extern void set_difference(struct set *s, struct set *t) {
    if(!set_test(s, t->b_array_bits-1)) {
        set_add(s, t->b_array_bits-1);
        set_remove(s, t->b_array_bits-1);
    }
    for(int i = 0; i < t->b_array_sz; i++) {
        s->b_array[i] ^= t->b_array[i];
    }
}

//struct set *, struct set * -> void
//perform an intersect of set s and t
//s hold the intersect
extern void set_intersect(struct set *s, struct set *t) {
    if(!set_test(s, t->b_array_bits-1)) {
        set_add(s, t->b_array_bits-1);
        set_remove(s, t->b_array_bits-1);
    }
    for(int i = 0; i < t->b_array_sz; i++) {
        s->b_array[i] &= t->b_array[i];
    }
        memset(s->b_array[t->b_array_sz], 0, 
                WORD*(s->b_array_sz-t->b_array_sz));
}

//struct set * -> struct set *
//duplicate set s
extern struct set *set_duplicate(struct set *s) {
    struct set *replica = set_create();
    if(s->b_array != s->def_b_array) {
        replica->b_array = malloc(s->b_array_sz*WORD);
    }
    memcpy(replica->b_array, s->b_array, WORD*s->b_array_sz);
    replica->b_array_sz = s->b_array_sz;
    replica->b_array_bits = s->b_array_bits;
    replica->complement = s->complement;
    return replica;
}

//struct set * -> void
//free set s' memory
extern void set_free(struct set *s) {
    if(s->b_array != s->def_b_array) {
        free(s->b_array);
    }
    free(s);
}

//struct set *, struct set * -> void
//assign members of set t to set s
extern void set_assign(struct set *s, struct set *t) {
    set_clear(s);
    set_union(s, t);
}

//struct set *, struct set * -> int
//compare sets s and t
//for s>t return +ve, s<t return -ve, s==t return 0
extern int set_compare(struct set *s, struct set *t) {
    if(!set_test(s, t->b_array_bits-1)) {
        set_add(s, t->b_array_bits-1);
        set_remove(s, t->b_array_bits-1);
    }
    for(int i = t->b_array_sz-1; i >= 0; i--) {
        if(s->b_array[i] != t->b_array[i]) {
            return s->b_array[i] - t->b_array[i];
        }
    }
    for(int i = t->b_array_sz; i < s->b_array_sz; i++) {
        if(s->b_array[i]) return s->b_array_sz;
    }
    return 0;
} 

//struct set *s, void(*)(int) -> void
//apply function f on each member of set s
extern void set_apply(struct set *s, void (*f)(int)) {
    int member = 0;
    while((member = set_nextmember(s)) != -1) {
        (*f)(member);
    }
}
//struct set *s -> int
//get next member of set s, -1 if set is depleted
extern int set_nextmember(struct set *s) {
    static struct set *active_set;
    static int active_set_i;
    if(!s) {
        active_set = s;
        return -1;
    }
    if(s != active_set) {
        active_set = s;
        active_set_i = 0;
        int i = 0;
        while(!active_set->b_array[i] && i < active_set->b_array_sz) {
            i++;
        }
        active_set_i =i*WORD_BITS;
    }
    while (active_set_i < active_set->b_array_bits) {
        if(set_member(active_set, active_set_i)) {
            return active_set_i++;
        }
        active_set++;
    }
    return -1;
}
