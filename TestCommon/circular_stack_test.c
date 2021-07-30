#include <stdio.h>

#include "../Common/circular_stack.h"

int main(int argc, char *argv[])
{
    struct stack *s;
    stack_create(&s, 3, sizeof(int));

    int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    stack_push(s, &input[0]);
    stack_push(s, &input[1]);
    stack_push(s, &input[2]);
    stack_push(s, &input[3]);
    stack_push(s, &input[4]);

    int out;
    stack_pop(s, &out);
    printf("%d\n", out);
    stack_pop(s, &out);
    printf("%d\n", out);
    stack_pop(s, &out);
    printf("%d\n", out);

    return 0;
}
