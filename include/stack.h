#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef int stack_type;

typedef enum StackError
{
    StackError_BAD_STACK   = -1,
    StackError_SUCCESS     =  0,
    StackError_ERROR_CTOR  =  1,
    StackError_ERROR_DTOR  =  2,
    StackError_EMPTY_STACK =  3,
} StackError;

typedef struct Stack
{
    stack_type* data;
    size_t      size_of_data;
    size_t      max_size_of_data;
} Stack;

StackError stackCtor(Stack* stack);
StackError stackDtor(Stack* stack);
StackError stackPush(Stack* stack, stack_type item);
StackError stackPop(Stack* stack, stack_type* item);

#endif // STACK_H
