//Bumbuna <developer@devbumbuna.com>
//2022
#include "include/bitset.h"
#include <assert.h>
#include <math.h>
#include <time.h>

#define fori(_i) for(int i = 0; i < _i; i++)
#define forsi(_seed,_i) for(int i = _seed; i < _i+_seed; i++)
#define max(a,b) (a>b ? a: b)
static int s_sz = 5;
static int t_sz = 20;

void test_union() {
    struct set *s = set_create();
    struct set *t = set_create();
    assert(s);
    assert(t);
    fori(s_sz) {
        set_add(s, i);
    }
    forsi(30, t_sz) {
        set_add(t, i);
    }
    assert(set_size(s) == s_sz);
    assert(set_size(t) == t_sz);
    set_union(s, t);
    assert(set_size(s)==(s_sz+t_sz));
    forsi(30, t_sz) {
        assert(set_test(s, i));
    }
    set_free(s);
    set_free(t);
}

void test_difference() {
    struct set *s = set_create(),
                *t = set_create();
    assert(s);
    assert(t);
    fori(s_sz) {
        set_add(s, i);
    }
    forsi(s_sz-5, t_sz) {
        set_add(t, i);
    }
    set_difference(s, t);
    assert(set_size(s) == (max(s_sz,t_sz)-5));
    forsi(s_sz-5, s_sz) {
        assert(!set_test(s, i));
    }
    set_free(s);
    set_free(t);
}

void test_intersect() {
    struct set *s = set_create(),
                *t = set_create();
    assert(s);
    assert(t);
    fori(s_sz) {
        set_add(s, i);
    }
    forsi(s_sz-5, t_sz) {
        set_add(t, i);
    }
    set_intersect(s, t);
    assert(set_size(s) == 5);
    forsi(s_sz-5, s_sz) {
        assert(set_test(s, i));
    }
    set_free(s);
    set_free(t);
}

void test_assign() {
    struct set *s = set_create();
    struct set *t = set_create();
    srand(time(NULL));
    while(set_size(s) != 10) {
        set_add(s, rand()%s->b_array_bits);
    }
    set_assign(t, s);
    assert(set_size(t) == set_size(s));
    assert(!set_compare(s, t));
}

void test_duplicate() {
    struct set *s = set_create();
    assert(s);
    struct set *t = set_duplicate(s);
    assert(set_size(s) == set_size(t));
    assert(!set_compare(s, t)); 
}

void test_nextmember() {
    struct set *s = set_create();
    assert(s);
    for(int i = 0; i < 9; i++) {
        set_add(s, i);
    }
    int m;
    int printed = 0;
    while((m = set_nextmember(s)) != -1) {
        printed += printf("%d", m);
    }
    assert(printed == 9);
}

int main(int argc, char **argv) {
    struct set *s = set_create();
    int iterations = 23;
    assert(s);
    fori(iterations) {
        set_add(s, i);
    }
    assert(set_size(s)==iterations);
    fori(iterations) {
        assert(set_member(s, i));
    }
    set_toggle_complement(s);
    fori(iterations) {
        assert(!set_member(s, i));
    }
    set_remove(s, iterations-1);
    assert(!set_test(s, iterations-1));
    set_add(s, iterations-1);
    set_invert(s);
    fori(iterations) {
        assert(!set_test(s, i));
    }
    set_clear(s);
    assert(set_size(s) == 0);
    test_union();
    test_intersect();
    test_difference();
    test_assign();
    test_duplicate();
    test_nextmember();
    set_free(s);
}
