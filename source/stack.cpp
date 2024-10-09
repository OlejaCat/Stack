#include "stack.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dump.h"
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

    const char* file_name;
    int         line;
    const char* function_name;

    uint8_t     data_canary_start[SIZE_OF_CANARY];
    uint8_t     data_canary_end[SIZE_OF_CANARY];
    hash_type   data_hash;
    hash_type   struct_hash;
    uint8_t     struct_canary_end[SIZE_OF_CANARY];
} Stack;

static const size_t SIZE_OF_STACK_FOR_HASH = sizeof(Stack) - 2 * sizeof(hash_type);

static const size_t START_SIZE   = 4;
static const size_t SCALE_FACTOR = 2;

static StackErrorOperation stackResize(Stack* stack, double scale_factor);


// public --------------------------------------------------------------------------------------------------------------


Stack* stackCtor_(const char* file_name,
                  int         line,
                  const char* function_name)
{
    Stack* stack = (Stack*)calloc(1, sizeof(Stack));

    generateCanaryRandom(stack->struct_canary_start);

    stack->data             = NULL;
    stack->size_of_element  = sizeof(stack_type);
    stack->size_of_data     = 0;
    stack->max_size_of_data = START_SIZE;
    stack->data_hash        = 0;

    generateCanaryRandom(stack->data_canary_start);
    generateCanaryRandom(stack->data_canary_end);

    memcpy(stack->struct_canary_start, stack->struct_canary_end, SIZE_OF_CANARY);

    stack_type* data_pointer = (stack_type*)canaryCalloc(START_SIZE,
                                                         stack->size_of_element,
                                                         stack->data_canary_start,
                                                         stack->data_canary_end);
    if (!data_pointer)
    {
        return NULL;
    }

    stack->data = data_pointer;

    stack->file_name     = file_name;
    stack->line          = line;
    stack->function_name = function_name;

    stack->struct_hash = calculateHash(stack, SIZE_OF_STACK_FOR_HASH);

    return stack;
}


StackErrorOperation stackDtor(Stack *stack)
{
    STACK_ASSERT(stack, StackErrorOperation_ERROR_DTOR);

    freeCanary(stack->data);
    free(stack);

    stack = NULL;

    return StackErrorOperation_SUCCESS;
}


StackErrorOperation stackPush(Stack *stack, stack_type item)
{
    STACK_ASSERT(stack, StackErrorOperation_ERROR_PUSH);

    stack->data[stack->size_of_data] = item;
    stack->size_of_data++;

    if (stack->size_of_data == stack->max_size_of_data)
    {
        stackResize(stack, SCALE_FACTOR);
    }

    stack->data_hash   = calculateHash(stack->data, stack->size_of_element * stack->max_size_of_data);
    stack->struct_hash = calculateHash(stack,       SIZE_OF_STACK_FOR_HASH);

    STACK_ASSERT(stack, StackErrorOperation_ERROR_PUSH);

    return StackErrorOperation_SUCCESS;
}


StackErrorOperation stackPop(Stack *stack, stack_type* item)
{
    STACK_ASSERT(stack, StackErrorOperation_ERROR_POP);

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

    stack->data_hash   = calculateHash(stack->data, stack->size_of_element * stack->max_size_of_data);
    stack->struct_hash = calculateHash(stack,       SIZE_OF_STACK_FOR_HASH);

    STACK_ASSERT(stack, StackErrorOperation_ERROR_POP);

    return StackErrorOperation_SUCCESS;
}


// static --------------------------------------------------------------------------------------------------------------


static StackErrorOperation stackResize(Stack* stack, double scale_factor)
{
    stack->max_size_of_data = (size_t)((double)stack->max_size_of_data * scale_factor);

    stack->data = (stack_type*)canaryRealloc(stack->data,
                                             stack->size_of_data * stack->size_of_element,
                                             stack->max_size_of_data * stack->size_of_element,
                                             stack->data_canary_start,
                                             stack->data_canary_end);

    return StackErrorOperation_SUCCESS;
}
