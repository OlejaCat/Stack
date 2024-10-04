#include "stack.h"

#include <stdbool.h>
#include <stdio.h>

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
    uint8_t     data_canary_start[SIZE_OF_CANARY];
    uint8_t     data_canary_end[SIZE_OF_CANARY];
    uint8_t     struct_canary_end[SIZE_OF_CANARY];
    hash_type   data_hash;
} Stack;


static const size_t START_SIZE   = 4;
static const size_t SCALE_FACTOR = 2;

static StackErrorOperation stackResizeUp(Stack* stack);
static StackErrorOperation stackResizeDown(Stack* stack);


// public --------------------------------------------------------------------------------------------------------------


Stack* stackCtor(size_t size_of_element)
{
    Stack* stack = (Stack*)calloc(1, sizeof(Stack));

    generateCanary(stack->struct_canary_start);

    stack->data             = NULL;
    stack->size_of_element  = size_of_element;
    stack->size_of_data     = 0;
    stack->max_size_of_data = START_SIZE;
    stack->data_hash        = 0;

    generateCanary(stack->data_canary_start);
    generateCanary(stack->data_canary_end);

    generateCanary(stack->struct_canary_end);

    stack_type* data_pointer = (stack_type*)canaryCalloc(START_SIZE,
                                                         stack->size_of_element,
                                                         stack->data_canary_start,
                                                         stack->data_canary_end);
    if (!data_pointer)
    {
        return NULL;
    }

    stack->data = data_pointer;

    return stack;
}


StackErrorOperation stackDtor(Stack *stack)
{
    STACK_ASSERT(stack, StackErrorOperation_ERROR_DTOR);

    freeCanary(stack->data);
    free(stack);

    return StackErrorOperation_SUCCESS;
}


StackErrorOperation stackPush(Stack *stack, stack_type item)
{
    STACK_ASSERT(stack, StackErrorOperation_ERROR_PUSH);

    stack->data[stack->size_of_data] = item;
    stack->size_of_data++;

    if (stack->size_of_data == stack->max_size_of_data)
    {
        stackResizeUp(stack);
    }

    stack->data_hash = calculateHash(stack->data, stack->size_of_element * stack->max_size_of_data);

    STACK_ASSERT(stack, StackErrorOperation_ERROR_PUSH);

    return StackErrorOperation_SUCCESS;
}


StackErrorOperation stackPop(Stack *stack, stack_type* item)
{
    if (stack->size_of_data == 0)
    {
        return StackErrorOperation_EMPTY_STACK;
    }

    STACK_ASSERT(stack, StackErrorOperation_ERROR_POP);

    *item = stack->data[stack->size_of_data - 1];

    stack->size_of_data--;
    stackResizeDown(stack);

    stack->data_hash = calculateHash(stack->data, stack->size_of_element * stack->max_size_of_data);

    STACK_ASSERT(stack, StackErrorOperation_ERROR_POP);

    return StackErrorOperation_SUCCESS;
}


// static --------------------------------------------------------------------------------------------------------------


static StackErrorOperation stackResizeUp(Stack* stack)
{
    stack->max_size_of_data *= SCALE_FACTOR;

    stack->data = (stack_type*)canaryRealloc(stack->data,
                                             stack->size_of_data * stack->size_of_element,
                                             stack->max_size_of_data * stack->size_of_element,
                                             stack->data_canary_start,
                                             stack->data_canary_end);

    return StackErrorOperation_SUCCESS;
}


static StackErrorOperation stackResizeDown(Stack* stack)
{
    if (stack->size_of_data * (SCALE_FACTOR * SCALE_FACTOR) == stack->max_size_of_data
     && stack->max_size_of_data != START_SIZE)
    {
        stack->max_size_of_data /= SCALE_FACTOR;

        stack->data = (stack_type*)canaryRealloc(stack->data,
                                                 stack->size_of_data * stack->size_of_element,
                                                 stack->max_size_of_data * stack->size_of_element,
                                                 stack->data_canary_start,
                                                 stack->data_canary_end);
    }

    return StackErrorOperation_SUCCESS;
}
