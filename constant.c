#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "constant.h"

char*type_name[] = {"undefined","float","category","int","bool","missing","array","string"};

array_t* array_new(int size)
{
    array_t*array = (array_t*)calloc(1, sizeof(array_t)+sizeof(constant_t)*size);
    array->size = size;
    return array;
}
array_t* array_create(int size, ...)
{
    va_list arglist;
    va_start(arglist, size);
    int t;
    array_t*array = array_new(size);
    for(t=0;t<size;t++) {
        array->entries[t] = category_constant(va_arg(arglist,category_t));
    }
    va_end(arglist);
    return array;
}

void array_destroy(array_t*a)
{
    free(a);
}

constant_t bool_constant(bool b)
{
    constant_t v;
    v.type = CONSTANT_BOOL;
    v.b = b;
    return v;
}
constant_t float_constant(float f)
{
    constant_t v;
    v.type = CONSTANT_FLOAT;
    v.f = f;
    return v;
}
constant_t missing_constant()
{
    constant_t v;
    v.type = CONSTANT_MISSING;
    return v;
}
constant_t category_constant(category_t c)
{
    constant_t v;
    v.type = CONSTANT_CATEGORY;
    v.c = c;
    return v;
}
constant_t int_constant(int i)
{
    constant_t v;
    v.type = CONSTANT_INT;
    v.i = i;
    return v;
}
constant_t array_constant(array_t*a)
{
    constant_t v;
    v.type = CONSTANT_ARRAY;
    v.a = a;
    return v;
}
constant_t string_constant(const char*s)
{
    constant_t v;
    v.type = CONSTANT_STRING;
    v.s = strdup(s);
    return v;
}
void constant_print(constant_t*v)
{
    int t;
    switch(v->type) {
        case CONSTANT_FLOAT:
            printf("%.2f", v->f);
        break;
        case CONSTANT_INT:
            printf("%d", v->i);
        break;
        case CONSTANT_CATEGORY:
            printf("C%d", v->c);
        break;
        case CONSTANT_BOOL:
            printf("%s", v->b?"true":"false");
        break;
        case CONSTANT_MISSING:
            printf("<missing>");
        break;
        case CONSTANT_ARRAY:
            printf("[");
            array_t*a = AS_ARRAY(*v);
            for(t=0;t<a->size;t++) {
                if(t>0)
                    printf(",");
                constant_print(&a->entries[t]);
            }
            printf("]");
        break;
        default:
            printf("<bad value>");
        break;
    }
}
void constant_clear(constant_t*v)
{
    switch(v->type) {
        case CONSTANT_ARRAY:
            free(v->a);
            v->a = 0;
        break;
        case CONSTANT_STRING:
            free(v->s);
            v->s = 0;
        break;
    }
}

int constant_check_type(constant_t v, uint8_t type)
{
    if(v.type!=type) {
        fprintf(stderr, "expected %s, got %s\n", type_name[type], type_name[v.type]);
    }
    assert(v.type == type);
    return 0;
}

