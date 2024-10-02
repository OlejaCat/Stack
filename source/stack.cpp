#include "stack.h"

#include <stdbool.h>

#include "my_asserts.h"

// static --------------------------------------------------------------------------------------------------------------

static const size_t START_SIZE   = 4;
static const size_t SCALE_FACTOR = 2;

static StackError stackWorkingState(Stack* stack);
static StackError stackResizeUp(Stack* stack);
static StackError stackResizeDown(Stack* stack);

// public --------------------------------------------------------------------------------------------------------------

StackError stackPush(Stack *stack, stack_type item)
{
    assertStrict(stackWorkingState(stack) == StackError_SUCCESS);

    stack->data[stack->size_of_data] = item;
    stack->size_of_data++;

    if (stack->size_of_data == stack->max_size_of_data)
    {
        stackResizeUp(stack);
    }

    assertStrict(stackWorkingState(stack) == StackError_SUCCESS);

    return StackError_SUCCESS;
}


StackError stackPop(Stack *stack, stack_type* item)
{
    if (stack->size_of_data == 0)
    {
        return StackError_EMPTY_STACK;
    }

    assertStrict(stackWorkingState(stack) == StackError_SUCCESS);

    *item = stack->data[stack->size_of_data - 1];

    stack->size_of_data--;
    stackResizeDown(stack);

    assertStrict(stackWorkingState(stack) == StackError_SUCCESS);

    return StackError_SUCCESS;
}


StackError stackCtor(Stack* stack)
{
    stack->data             = NULL;
    stack->size_of_data     = 0;
    stack->max_size_of_data = START_SIZE;

    stack_type* data_pointer = (stack_type*)calloc(START_SIZE, sizeof(stack_type));
    if (!data_pointer)
    {
        return StackError_ERROR_CTOR;
    }

    stack->data = data_pointer;

    assertStrict(stackWorkingState(stack) == StackError_SUCCESS);
    return StackError_SUCCESS;
}


StackError stackDtor(Stack *stack)
{
    if (stackWorkingState(stack) == StackError_BAD_STACK)
    {
        return StackError_ERROR_DTOR;
    }

    free(stack->data);

    return StackError_SUCCESS;
}


// static --------------------------------------------------------------------------------------------------------------

static StackError stackWorkingState(Stack* stack)
{
    bool size_of_data_too_big  = stack->size_of_data > stack->max_size_of_data;
    bool null_pointer_to_data  = stack->data == NULL;

    return size_of_data_too_big || null_pointer_to_data
            ? StackError_BAD_STACK
            : StackError_SUCCESS;
}

static StackError stackResizeUp(Stack* stack)
{
    stack->max_size_of_data *= SCALE_FACTOR;

    stack->data = (stack_type*)realloc(stack->data, stack->max_size_of_data * sizeof(stack_type));

    return StackError_SUCCESS;
}

static StackError stackResizeDown(Stack* stack)
{
    if (stack->size_of_data * (SCALE_FACTOR * SCALE_FACTOR) == stack->max_size_of_data
     && stack->max_size_of_data != START_SIZE)
    {
        stack->max_size_of_data /= SCALE_FACTOR;

        stack->data = (stack_type*)realloc(stack->data, stack->max_size_of_data * sizeof(stack_type));
    }

    return StackError_SUCCESS;
}
