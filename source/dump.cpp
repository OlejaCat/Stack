#include "dump.h"

#include <stdio.h>
#include <assert.h>

#include "canary_protection.h"
#include "hash_protection.h"
#include "string_color.h"


// static --------------------------------------------------------------------------------------------------------------

typedef struct Stack
{
#ifdef _CANARY_PROTECT
    uint8_t     struct_canary_start[SIZE_OF_CANARY];
#endif

    stack_type* data;
    size_t      size_of_element;
    size_t      size_of_data;
    size_t      max_size_of_data;

    const char* file_name;
    int         line;
    const char* function_name;

#ifdef _CANARY_PROTECT
    uint8_t     data_canary_start[SIZE_OF_CANARY];
    uint8_t     data_canary_end[SIZE_OF_CANARY];
#endif

#ifdef _HASH_PROTECT
    hash_type   data_hash;
    hash_type   struct_hash;
#endif

#ifdef _CANARY_PROTECT
    uint8_t     struct_canary_end[SIZE_OF_CANARY];
#endif
} Stack;

static const size_t SIZE_OF_STACK_FOR_HASH = sizeof(Stack) - 2 * sizeof(hash_type);

static StackError stackWorkingState(Stack* stack);
static const char* stringifyStackErrorOperation(const StackErrorOperation stack_error_operation);
static const char* stringifyStackError(const StackError stack_error);


// public --------------------------------------------------------------------------------------------------------------


int stackErrorProcess(Stack*              stack,
                      StackErrorOperation operation_error,
                      const char*         stack_variable_name,
                      CallData            call_data)
{
    assert(call_data.file_name     != NULL);
    assert(call_data.function_name != NULL);
    assert(stack                   != NULL);

    if (!stack)
    {
        return EXIT_FAILURE;
    }

    StackError stack_error = stackWorkingState(stack);

    if (stack_error == StackError_SUCCESS)
    {
        return EXIT_SUCCESS;
    }

    writeStackDumpLog_(stack,
                       stack_error,
                       operation_error,
                       stack_variable_name,
                       call_data);

    return EXIT_FAILURE;
}


int writeStackDumpLog_(Stack*              stack,
                       StackError          stack_error,
                       StackErrorOperation stack_error_operation,
                       const char*         stack_variable_name,
                       CallData            call_data)
{
    assert(call_data.file_name     != NULL);
    assert(call_data.function_name != NULL);
    assert(stack                   != NULL);

    FILE* output_file = fopen("dump.txt", "w");
    if (!output_file)
    {
        fprintf(stderr, BOLD_YELLOW "Error opening stack dump file" RESET);
        return EXIT_FAILURE;
    }

    fprintf(output_file, "Stack variable \"%s\"\nCreated in %s:%d, function: %s\n",
                         stack_variable_name,
                         stack->file_name,
                         stack->line,
                         stack->function_name);

    if (stack == NULL)
    {
        fprintf(output_file, "No pointer to stak");
        return EXIT_SUCCESS;
    }

    if (stack_error == StackError_SUCCESS)
    {
        fprintf(output_file, "No problems with the stack were found.\n\n");
    }
    else
    {
    fprintf(output_file, "Error code: %s: %s:%d In function: %s. Operation code error: %s\n\n",
                         stringifyStackError(stack_error),
                         call_data.file_name,
                         call_data.line_number,
                         call_data.function_name,
                         stringifyStackErrorOperation(stack_error_operation));
    }

    fprintf(output_file, "Struct information [%p]\n", stack);

#ifdef _CANARY_PROTECT
    fprintf(output_file, "\tstruct_canary_start [%p]\n", stack->struct_canary_start);
#endif
    fprintf(output_file, "\tsize_of_element   = %lu\n", stack->size_of_element);
    fprintf(output_file, "\tsize_of_data      = %lu\n", stack->size_of_data);
    fprintf(output_file, "\tmax_size_of_data  = %lu\n", stack->max_size_of_data);
#ifdef _CANARY_PROTECT
    fprintf(output_file, "\tdata_canary_start   [%p]\n"  , stack->data_canary_start);
    fprintf(output_file, "\tdata_canary_end     [%p]\n"    , stack->data_canary_end);
#endif
#ifdef _HASH_PROTECT
    fprintf(output_file, "\tdata_hash         = %llu\n" , stack->data_hash);
    fprintf(output_file, "\tstruct_hash       = %llu\n" , stack->struct_hash);
#endif
#ifdef _CANARY_PROTECT
    fprintf(output_file, "\tstruct_canary_end   [%p]\n\n", stack->struct_canary_end);
#endif

    if (stack->data == NULL || stack->size_of_data > stack->max_size_of_data)
    {
        fprintf(output_file, "Data pointer is NULL");
    }
    else
    {
        fprintf(output_file, "Data [%p]\n", stack->data);

        if (stack->size_of_data == 0)
        {
            fprintf(output_file, "\tStack is empty\n");
        }

        for (size_t index = 0; index < stack->size_of_data; index++)
        {
            fprintf(output_file, "\t%lu: %lg [%p]\n",
                    index,
                    stack->data[index],
                    stack->data + index * stack->size_of_element);
        }
    }

    fclose(output_file);

    return EXIT_SUCCESS;
}




// static --------------------------------------------------------------------------------------------------------------


static StackError stackWorkingState(Stack* stack)
{
    assert(stack != NULL);

#define RETURN_ERROR_(condition, answer, error) \
    if (condition == answer) \
    { \
        return error; \
    }

    bool null_stack = (stack == NULL);
    RETURN_ERROR_(null_stack, 1, StackError_DATA_NULL_POINTER)

    bool size_of_data_too_big = (stack->size_of_data > stack->max_size_of_data);
    RETURN_ERROR_(size_of_data_too_big, 1, StackError_SIZE_OUT_BOUNDS)

    bool null_pointer_to_data  = (stack->data == NULL);
    RETURN_ERROR_(null_pointer_to_data, 1, StackError_DATA_NULL_POINTER)

    CanaryProtectionState canary_struct_broken = checkStructCanaries(stack->struct_canary_start,
                                                                     stack->struct_canary_end);
    RETURN_ERROR_(canary_struct_broken, CanaryProtectionState_CORRUPTED, StackError_BAD_STRUCT_CANARY)

    HashProtectionState hash_struct_broken = checkHash(stack->struct_hash,
                                                       stack,
                                                       SIZE_OF_STACK_FOR_HASH);
    RETURN_ERROR_(hash_struct_broken, HashProtectionState_CORRUPTED, StackError_BAD_STRUCT_HASH)

    CanaryProtectionState canary_data_broken = checkDataCanaries(stack->data,
                                                                 stack->max_size_of_data * stack->size_of_element,
                                                                 stack->data_canary_start,
                                                                 stack->data_canary_end);
    RETURN_ERROR_(canary_data_broken, CanaryProtectionState_CORRUPTED, StackError_BAD_DATA_CANARY)

    HashProtectionState hash_data_broken = checkHash(stack->data_hash,
                                                     stack->data,
                                                     stack->max_size_of_data * stack->size_of_element);
    RETURN_ERROR_(hash_data_broken, HashProtectionState_CORRUPTED, StackError_BAD_DATA_HASH)

#undef RETURN_ERROR_

    return StackError_SUCCESS;
}


static const char* stringifyStackErrorOperation(const StackErrorOperation stack_error_operation)
{
#define STRINGIFY_MAKE_CASE_(name_of_case) \
    case name_of_case: return #name_of_case

#define STRINGIFY_DEFAULT_CASE_(name_of_case) \
    default: return #name_of_case

    switch (stack_error_operation)
    {
        STRINGIFY_MAKE_CASE_(StackErrorOperation_SUCCESS);
        STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_DTOR);
        STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_CTOR);
        STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_PUSH);
        STRINGIFY_MAKE_CASE_(StackErrorOperation_ERROR_POP);
        STRINGIFY_MAKE_CASE_(StackErrorOperation_EMPTY_STACK);

        STRINGIFY_DEFAULT_CASE_(StackErrorOperation_UNKNOWN);
    }

#undef STRINGIFY_MAKE_CASE_
#undef STRINGIFY_DEFAULT_CASE_
}


static const char* stringifyStackError(const StackError stack_error)
{
#define STRINGIFY_MAKE_CASE_(name_of_case) \
    case name_of_case: return #name_of_case

#define STRINGIFY_DEFAULT_CASE_(name_of_case) \
    default: return #name_of_case

    switch (stack_error)
    {
        STRINGIFY_MAKE_CASE_(StackError_SUCCESS);
        STRINGIFY_MAKE_CASE_(StackError_ERROR);
        STRINGIFY_MAKE_CASE_(StackError_BAD_DATA_CANARY);
        STRINGIFY_MAKE_CASE_(StackError_BAD_DATA_HASH);
        STRINGIFY_MAKE_CASE_(StackError_BAD_STRUCT_CANARY);
        STRINGIFY_MAKE_CASE_(StackError_BAD_STRUCT_HASH);
        STRINGIFY_MAKE_CASE_(StackError_DATA_NULL_POINTER);
        STRINGIFY_MAKE_CASE_(StackError_STACK_NULL_POINTER);
        STRINGIFY_MAKE_CASE_(StackError_SIZE_OUT_BOUNDS);

        STRINGIFY_DEFAULT_CASE_(StackError_UNKNOWN);
    }

#undef STRINGIFY_MAKE_CASE_
#undef STRINGIFY_DEFAULT_CASE_
}
