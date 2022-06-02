//Bumbuna <developer@devbumbuna.com>
//2022
#include "include/stack.h"
#include <assert.h>

#undef stack_class
#define stack_class static

int main() {
    stack_create(nums, 32,int);
    assert(stack_isempty(nums));
    stack_push(nums, 0);
    stack_push(nums, 1);
    assert(stack_pop(nums)==1);
    assert(stack_peek(nums) == 0);
    stack_pop(nums);
    assert(stack_isempty(nums));
}

#undef stack_class
