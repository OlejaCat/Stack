#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>

typedef int stack_type;

typedef enum StackErrorOperation
{
    StackErrorOperation_SUCCESS     = 0,
    StackErrorOperation_ERROR_CTOR  = 1,
    StackErrorOperation_ERROR_DTOR  = 2,
    StackErrorOperation_ERROR_PUSH  = 3,
    StackErrorOperation_ERROR_POP   = 4,
    StackErrorOperation_EMPTY_STACK = 5,
    StackErrorOperation_UNKNOWN     = 6,
} StackErrorOperation;

typedef struct Stack Stack;

#define stackCtor() stackCtor_(__FILE__, __LINE__, __PRETTY_FUNCTION__)

Stack* stackCtor_(const char* file_name,
                  int         line,
                  const char* function_name);
StackErrorOperation stackDtor(Stack* stack);
StackErrorOperation stackPush(Stack* stack, stack_type item);
StackErrorOperation stackPop(Stack* stack, stack_type* item);

#endif // STACK_H
