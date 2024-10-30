#ifndef DUMP_H
#define DUMP_H

#include "stack.h"
#include "my_asserts.h"

#define STACK_ASSERT(stack_struct, error, information_about_file) \
    if (stackErrorProcess(stack_struct, error, #stack_struct, \
        information_about_file) != EXIT_SUCCESS) \
    { \
      abortWithMessage("Something went wrong with stack check dump.txt for info");  \
      return error;  \
    }

#define writeStackDumpLog(stack_struct) \
    writeStackDumpLog_(stack_struct, StackError_SUCCESS, StackErrorOperation_SUCCESS, #stack_struct, \
                       {NULL, 0, NULL});

typedef enum StackError
{
    StackError_ERROR              = -1,
    StackError_SUCCESS            =  0,
    StackError_BAD_STRUCT_CANARY  =  1,
    StackError_BAD_STRUCT_HASH    =  2,
    StackError_BAD_DATA_CANARY    =  3,
    StackError_BAD_DATA_HASH      =  4,
    StackError_DATA_NULL_POINTER  =  5,
    StackError_STACK_NULL_POINTER =  6,
    StackError_SIZE_OUT_BOUNDS    =  7,
    StackError_UNKNOWN            =  8,
} StackError;

int stackErrorProcess(Stack*              stack,
                      StackErrorOperation operation_error,
                      const char*         stack_variable_name,
                      CallData            call_data);

int writeStackDumpLog_(Stack*              stack,
                       StackError          stack_error,
                       StackErrorOperation stack_error_operation,
                       const char*         stack_variable_name,
                       CallData            call_data);

#endif // DUMP_H
