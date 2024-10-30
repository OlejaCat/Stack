#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>

typedef double stack_type;

typedef struct CallData
{
    const char* file_name;
    const int   line_number;
    const char* function_name;
} CallData;

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

#define stackCtor() stackCtor_({__FILE__, __LINE__, __PRETTY_FUNCTION__})

#define stackDtor(stack_struct) stackDtor_(stack_struct)

#define stackPush(stack_struct, item_to_push) stackPush_(stack_struct, item_to_push, \
                                                         {__FILE__, __LINE__, __PRETTY_FUNCTION__})

#define stackPop(stack_struct, item_to_pop) stackPop_(stack_struct, item_to_pop, \
                                                      {__FILE__, __LINE__, __PRETTY_FUNCTION__})

Stack* stackCtor_(CallData call_data);

StackErrorOperation stackDtor_(Stack*   stack,
                               CallData call_data);

StackErrorOperation stackPush_(Stack*     stack,
                               stack_type item,
                               CallData   call_data);

StackErrorOperation stackPop_(Stack*      stack,
                              stack_type* item,
                              CallData    call_data);

#endif // STACK_H
