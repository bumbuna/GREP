//Bumbuna <developer@devbumbuna.com>
//2022
#ifndef STACK_H
#define STACK_H

#define stack_class

#define stack_create(name,size, type) \
    typedef type name##_t; \
    stack_class type s##name[size]; \
    stack_class type *s##name##ptr = s##name;

#define stack_push(name, x) \
    *s##name##ptr++ = x

#define stack_pop(name) \
    *--s##name##ptr

#define stack_peek(name) \
    *(s##name##ptr-1)
 
#define stack_error(x) \
    (name##_t) fprintf(stderr, "%s\n", x ? "Overflow", "Underflow");

#define stack_clear(name) \
    s##name##ptr = s##name

#define stack_isempty(name) \
    (s##name##ptr == s##name)

#define stack_isoverflow(name) \
    (s##name##ptr > &s##name[sizeof(s##name)/sizeof(name##_t)])

#define stack_isunderflow(name) \
    (s##name##ptr < s##name)
    
#endif //STACK_H
