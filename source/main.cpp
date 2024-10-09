#include <stdio.h>

#include "stack.h"
#include "logger.h"


int main()
{
    openLogFile("logger.txt");

    Stack* st = stackCtor();

    uint8_t* ptr = (uint8_t*) st;

    *(ptr + 15) = 10;

    for (uint8_t i = 0; i < 220; i++)
    {
        stackPush(st, i);
    }

    for (int i = 0; i < 200; i++)
    {
        stack_type x = 0;
        stackPop(st, &x);
        printf("%d: %d\n", i + 1, x);
    }

    stackDtor(st);
}
