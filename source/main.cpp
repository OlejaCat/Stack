#include <stdio.h>

#include "stack.h"
#include "logger.h"


int main()
{
    openLogFile("logger.txt");

    Stack st = {};
    stackCtor(&st);

    Log(LogLevel_DEBUG, "pointer: %p, size: %d, max_size: %d", st.data, st.size_of_data, st.max_size_of_data);
    for (int i = 0; i < 16; i++)
    {
        Log(LogLevel_DEBUG, "i entered: %d", i);
        stackPush(&st, 3802 + i);
    }

    for (int i = 0; i < 16; i++)
    {
        stack_type x = 0;
        stackPop(&st, &x);
        printf("%d\n", x);
    }

    stackDtor(&st);
}
