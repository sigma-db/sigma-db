#include <stdio.h>

#include "../Common/circular_stack.h"

int main(int argc, char *argv[])
{
    struct circular_stack *s;
    circular_stack_create(&s, 3, sizeof(int));

    int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    circular_stack_push(s, &input[0]);
    circular_stack_push(s, &input[1]);
    circular_stack_push(s, &input[2]);
    circular_stack_push(s, &input[3]);
    circular_stack_push(s, &input[4]);

    int out;
    circular_stack_pop(s, &out);
    printf("%d\n", out);
    circular_stack_pop(s, &out);
    printf("%d\n", out);
    circular_stack_pop(s, &out);
    printf("%d\n", out);

    return 0;
}
