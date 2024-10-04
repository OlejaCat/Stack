#include <stdio.h>

#include "stack.h"
#include "logger.h"


int main()
{
    openLogFile("logger.txt");

    Stack* st = stackCtor(sizeof(int));

    for (int i = 0; i < 257; i++)
    {
        Log(LogLevel_DEBUG, "i entered: %d", i);
        stackPush(st, 3802 + i);
    }

    for (int i = 0; i < 257; i++)
    {
        stack_type x = 0;
        stackPop(st, &x);
        printf("%d: %d\n", i + 1, x);
    }

    stackDtor(st);
}
