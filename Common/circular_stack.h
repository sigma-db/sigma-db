#pragma once

#include <stdlib.h>

struct stack;

int stack_create(struct stack **s, size_t length, size_t item_sz);

void stack_destroy(struct stack *s);

int stack_push(struct stack *s, void *item);

int stack_pop(struct stack *s, void *item);
