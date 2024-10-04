#include "dump.h"

#include <stdio.h>

#include "my_asserts.h"
#include "canary_protection.h"
#include "hash_protection.h"


// static --------------------------------------------------------------------------------------------------------------


typedef struct Stack
{
    uint8_t     struct_canary_start[SIZE_OF_CANARY];
    stack_type* data;
    size_t      size_of_element;
    size_t      size_of_data;
    size_t      max_size_of_data;
    uint8_t     data_canary_start[SIZE_OF_CANARY];
    uint8_t     data_canary_end[SIZE_OF_CANARY];
    uint8_t     struct_canary_end[SIZE_OF_CANARY];
    hash_type   data_hash;
} Stack;


static StackError stackWorkingState(Stack* stack);
static const char* stackStringifyError(StackErrorOperation stack_error);


// public --------------------------------------------------------------------------------------------------------------


int stackAssertFunction(Stack*              stack,
                        StackErrorOperation operation_error,
                        const char*         file_name,
                        const int           line_number,
                        const char*         function_name)
{
    assertStrict(file_name     != NULL);
    assertStrict(function_name != NULL);

    if (!stack)
    {
        return EXIT_FAILURE;
    }

    if (stackWorkingState(stack) == StackError_SUCCESS)
    {
        return EXIT_SUCCESS;
    }

    FILE* input_file = fopen("dump.txt", "w");
    if (!input_file)
    {
        fprintf(input_file, "Error opening dump file");

        return EXIT_FAILURE;
    }

    fprintf(input_file, "%s: %s:%d In function: %s\n",
                         stackStringifyError(operation_error),
                         file_name,
                         line_number,
                         function_name);
    fprintf(input_file, "Struct information [%p]\n", stack);

    fprintf(input_file, "\tstruct_canary_start [%p]\n", stack->struct_canary_start);
    fprintf(input_file, "\tsize_of_element  = [%lu]\n", stack->size_of_element);
    fprintf(input_file, "\tsize_of_data     = [%lu]\n", stack->size_of_data);
    fprintf(input_file, "\tmax_size_of_data = [%lu]\n", stack->max_size_of_data);
    fprintf(input_file, "\tdata_canary_start [%p]\n",   stack->data_canary_start);
    fprintf(input_file, "\tdata_canary_end [%p]\n",     stack->data_canary_end);
    fprintf(input_file, "\tdata_hash = %llu\n",         stack->data_hash);

    fclose(input_file);

    return EXIT_FAILURE;
}


// static --------------------------------------------------------------------------------------------------------------


static StackError stackWorkingState(Stack* stack)
{
    #define __FAST_IF__(condition, answer) \
        if (condition == answer) \
        { \
            return StackError_BAD_STACK; \
        }

    bool size_of_data_too_big  = stack->size_of_data > stack->max_size_of_data;
    bool null_pointer_to_data  = stack->data == NULL;

    __FAST_IF__(size_of_data_too_big || null_pointer_to_data, 1)

    CanaryProtectionState canary_broken = checkDataCanaries(stack->data,
                                                            stack->max_size_of_data * stack->size_of_element,
                                                            stack->data_canary_start,
                                                            stack->data_canary_end);

    __FAST_IF__(canary_broken, CanaryProtectionState_CORRUPTED)

    HashProtectionState hash_broken = checkDataHash(stack->data_hash,
                                                    stack->data,
                                                    stack->max_size_of_data * stack->size_of_element);

    __FAST_IF__(hash_broken, HashProtectionState_CORRUPTED)

    return StackError_SUCCESS;

    #undef __FAST_IF__
}


static const char* stackStringifyError(const StackErrorOperation stack_error)
{
    #define _STRINGIFY_MAKE_CASE_(name_of_case) \
        case name_of_case: return #name_of_case

    #define _STRINGIFY_DEFAULT_CASE_(name_of_case) \
        default: return #name_of_case

    switch (stack_error)
    {
        _STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_DTOR);
        _STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_CTOR);
        _STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_PUSH);
        _STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_POP);
        _STRINGIFY_MAKE_CASE_(StackErrorOperation_EMPTY_STACK);
        _STRINGIFY_DEFAULT_CASE_(StackErrorOperation_UNKNOWN);
    }

    #undef _STRINGIFY_MAKE_CASE_
    #undef _STRINGIFY_DEFAULT_CASE_
}
