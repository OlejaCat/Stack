#ifndef DUMP_H
#define DUMP_H

#include "stack.h"
#include "my_asserts.h"

#define STACK_ASSERT(stack, error) \
    if (stackAssertFunction(stack, error, __FILE__, __LINE__, __PRETTY_FUNCTION__) != EXIT_SUCCESS) \
    { \
      assertStrict(0 && "Out");  \
      return error;  \
    }

typedef enum StackError
{
    StackError_SUCCESS         = 0,
    StackError_BAD_STACK       = 1,
    StackError_NULL_POINTER    = 2,
    StackError_SIZE_OUT_BOUNDS = 3,
} StackError;


int stackAssertFunction(Stack*              stack,
                        StackErrorOperation operation_error,
                        const char*         file_name,
                        const int           line_number,
                        const char*         function_name);

#endif // DUMP_H
