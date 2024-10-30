#include "stack.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "canary_protection.h"
#include "hash_protection.h"
#include "helpful_functions.h"
#include "dump.h"


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

#ifdef _HASH_PROTECT
static const size_t SIZE_OF_STACK_FOR_HASH = sizeof(Stack) - 2 * sizeof(hash_type);
#endif

static const size_t START_SIZE   = 4;
static const size_t SCALE_FACTOR = 2;

static StackErrorOperation stackResize(Stack* stack, double scale_factor);

static void initializeCanaries(Stack* stack)  __attribute__((unused));


// public --------------------------------------------------------------------------------------------------------------


Stack* stackCtor_(CallData call_data)
{
    Stack* stack = (Stack*)calloc(1, sizeof(Stack));

#ifdef _CANARY_PROTECT
    initializeCanaries(stack);
#endif

    stack->data             = NULL;
    stack->size_of_element  = sizeof(stack_type);
    stack->size_of_data     = 0;
    stack->max_size_of_data = START_SIZE;

#ifdef _HASH_PROTECT
    stack->data_hash = 0;
#endif

    stack_type* data_pointer = (stack_type*)canaryCalloc(START_SIZE,
                                                         stack->size_of_element,
                                                         stack->data_canary_start,
                                                         stack->data_canary_end);
    if (!data_pointer)
    {
        return NULL;
    }

    stack->data = data_pointer;

    stack->file_name     = call_data.file_name;
    stack->line          = call_data.line_number;
    stack->function_name = call_data.function_name;

#ifdef _HASH_PROTECT
    stack->struct_hash = calculateHash(stack, SIZE_OF_STACK_FOR_HASH);
#endif

    return stack;
}


StackErrorOperation stackDtor_(Stack* stack)
{
    assert(stack != NULL);

    if (stack == NULL)
    {
        return StackErrorOperation_ERROR_DTOR;
    }

    freeCanary(stack->data);
    FREE_NULL(stack);

    return StackErrorOperation_SUCCESS;
}


StackErrorOperation stackPush_(Stack*     stack,
                               stack_type item,
                               CallData   call_data)
{
    assert(stack != NULL);

    STACK_ASSERT(stack, StackErrorOperation_ERROR_PUSH, call_data);

    stack->data[stack->size_of_data] = item;
    stack->size_of_data++;

    if (stack->size_of_data == stack->max_size_of_data)
    {
        stackResize(stack, SCALE_FACTOR);
    }

#ifdef _HASH_PROTECT
    stack->data_hash   = calculateHash(stack->data, stack->size_of_element * stack->max_size_of_data);
    stack->struct_hash = calculateHash(stack,       SIZE_OF_STACK_FOR_HASH);
#endif

    STACK_ASSERT(stack, StackErrorOperation_ERROR_PUSH, call_data);

    return StackErrorOperation_SUCCESS;
}


StackErrorOperation stackPop_(Stack*      stack,
                              stack_type* item,
                              CallData    call_data)
{
    assert(stack != NULL);
    assert(item  != NULL);

    STACK_ASSERT(stack, StackErrorOperation_ERROR_POP, call_data);

    if (stack->size_of_data == 0)
    {
        return StackErrorOperation_EMPTY_STACK;
    }

    *item = stack->data[stack->size_of_data - 1];

    stack->size_of_data--;

    if (stack->size_of_data * (SCALE_FACTOR * SCALE_FACTOR) == stack->max_size_of_data
     && stack->max_size_of_data != START_SIZE)
    {
        stackResize(stack, 1.0 / SCALE_FACTOR);
    }

#ifdef _HASH_PROTECT
    stack->data_hash   = calculateHash(stack->data, stack->size_of_element * stack->max_size_of_data);
    stack->struct_hash = calculateHash(stack,       SIZE_OF_STACK_FOR_HASH);
#endif

    STACK_ASSERT(stack, StackErrorOperation_ERROR_POP, call_data);

    return StackErrorOperation_SUCCESS;
}


// static --------------------------------------------------------------------------------------------------------------


static StackErrorOperation stackResize(Stack* stack, double scale_factor)
{
    assert(stack != NULL);

    stack->max_size_of_data = (size_t)((double)stack->max_size_of_data * scale_factor);

    stack->data = (stack_type*)canaryRealloc(stack->data,
                                             stack->size_of_data * stack->size_of_element,
                                             stack->max_size_of_data * stack->size_of_element,
                                             stack->data_canary_start,
                                             stack->data_canary_end);

    return StackErrorOperation_SUCCESS;
}


static void initializeCanaries(__attribute__((unused)) Stack* stack)
{
    generateCanaryRandom(stack->struct_canary_start);

    generateCanaryRandom(stack->data_canary_start);
    generateCanaryRandom(stack->data_canary_end);

#ifdef _CANARY_PROTECT
    memcpy(stack->struct_canary_start, stack->struct_canary_end, SIZE_OF_CANARY);
#endif
}
