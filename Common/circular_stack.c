#include <stdlib.h>
#include <string.h>

struct stack {
    size_t length;
    size_t stride;
    size_t item_cnt;
    size_t pos;
    char data[];
};

int stack_create(struct stack **s, size_t length, size_t item_sz)
{
    *s = malloc(sizeof(struct stack) + length * item_sz);
    if (*s == NULL) {
        return -1;
    }
    **s = (struct stack){
        .length = length,
        .stride = item_sz,
        .item_cnt = 0,
        .pos = 0
    };
    return 0;
}

inline void stack_destroy(struct stack *s)
{
    free(s);
}

int stack_push(struct stack *s, void *item)
{
    void *data_offset = &s->data[s->pos * s->stride];
    errno_t err = memcpy_s(data_offset, s->stride, item, s->stride);
    if (err) {
        return -1;
    }
    s->pos = (s->pos + 1) % s->length;
    s->item_cnt++;
    return 0;
}

int stack_pop(struct stack *s, void *item)
{
    if (s->item_cnt == 0) {
        return -1;
    }
    size_t item_pos = ((int)s->pos - 1) % (int)s->length;
    void *data_offset = &s->data[item_pos * s->stride];
    errno_t err = memcpy_s(item, s->stride, data_offset, s->stride);
    if (err) {
        return -1;
    }
    s->pos = item_pos;
    s->item_cnt--;
    return 0;
}
