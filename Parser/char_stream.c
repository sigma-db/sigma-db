#include <stdlib.h>
#include <assert.h>

#include "char_stream.h"

#define iteration_invariant(it) \
    ((it)->begin <= (it)->current && ((it)->end == NULL || (it)->current < (it)->end))

// Checks whether `end != NULL -> current < end` holds
// That is, if the string's size is known in advance, the current position must be strictly before the end
#define iterator_has_more(it) \
    ((it)->end == NULL || (it)->current < (it)->end) // TODO: check if \0 for null-terminated

#define index(cs, k) \
    (((cs)->pos + k) % (CHAR_STREAM_LOOKAHEAD + 1))

#define current(cs) \
    ((cs)->cache[(cs)->pos])

#ifdef PARSER_UTF8_ENABLED
#define code_point_size(c) \
    (c < 0x80 ? 1 : c < 0x800 ? 2 : c < 0x10000 ? 3 : 4)
#endif // PARSER_UTF8_ENABLED

struct char_stream {
    // iterator fields
    const char *begin;
    const char *end;
    const char *current; // invariant: begin <= buf < end (with end = 0 meaning infty)

    // char stream-specific fields
    size_t pos; // index into `values`, i.e. 0 <= pos <= CHAR_STREAM_LOOKAHEAD
    sigma_char_t cache[CHAR_STREAM_LOOKAHEAD + 1];
};

static sigma_char_t next_code_point(struct char_stream *cs);

int char_stream_create(struct char_stream **cs, const char *buf, size_t bufsz)
{
    assert(cs != NULL);
    assert(buf != NULL);

    *cs = malloc(sizeof(struct char_stream));
    if (*cs == NULL) {
        return -1;
    }
    **cs = (struct char_stream){
        .begin = buf,
        .end = bufsz > 0 ? buf + bufsz : NULL,
        .current = buf,
        .pos = 0,
        .cache = { 0 },
    };

    assert(iteration_invariant(*cs));

    char_stream_advance(*cs, CHAR_STREAM_LOOKAHEAD + 1);
    (*cs)->pos = 0;

    return 0;
}

void char_stream_destroy(struct char_stream *cs)
{
    free(cs);
}

sigma_char_t char_stream_next(struct char_stream *cs)
{
    assert(current(cs) > 0);

    sigma_char_t value = current(cs);
    char_stream_advance(cs, 1);
    return value;
}

sigma_char_t char_stream_peek(struct char_stream *cs, size_t k)
{
    assert(0 < k && k <= CHAR_STREAM_LOOKAHEAD);
    return cs->cache[index(cs, k)];
}

size_t char_stream_advance(struct char_stream *cs, size_t delta)
{
    /*
     * I considered a special treatment for k = 1, e.g. using conditional compilation,
     * but apart from the two superfluous checks for i < k, there does not seem to be
     * any overhead in the current solution that would justify the obfuscation
     */

    int i = 0; // number of code points read
    while (i < delta && iterator_has_more(cs)) { // BUG: we might still skip within the buffer although the iterator is done
        if ((cs->cache[cs->pos] = next_code_point(cs)) < 0) {
            break;
        }
        cs->pos = index(cs, 1);
        i += 1;
    }
    // 2nd loop to solve above bug?

    assert(iteration_invariant(cs) || cs->current == cs->end); // in case we're done, we must be exactly one past the last element

    return i;
}

size_t char_stream_pos(struct char_stream *cs)
{
#ifdef PARSER_UTF8_ENABLED
    // Subtract widths of all cached code points from (cs->current - cs->begin)
    size_t sum = 0;
    for (size_t i = 0; i <= CHAR_STREAM_LOOKAHEAD; i++) {
        sum += code_point_size(cs->cache[i]);
    }
    return cs->current - cs->begin - sum;
#else
    return cs->current - cs->begin;
#endif // PARSER_UTF8_ENABLED
}

bool char_stream_has_more(struct char_stream *cs)
{
    return current(cs) > 0;
}

// Returns the next byte from the input and forwards by one byte
// Make sure this function is inlined in the assembly
static char next_byte(struct char_stream *cs)
{
    assert(iterator_has_more(cs));
    return *cs->current++;
}

// TODO: Verify this function is referenced only once and thus gets inlined as well
static sigma_char_t next_code_point(struct char_stream *cs)
{
    // 1 byte (ASCII)
    sigma_char_t c = next_byte(cs);
    if (c < 0x80) {
        return c;
    }
#ifdef PARSER_UTF8_ENABLED
    // 2 bytes
    c = (c << 6) | (next_byte(cs) & 0x3F);
    if (c < 0x00003800) {
        return c & 0x000007FF; // (1 << 11) - 1
    }
    // 3 bytes
    c = (c << 6) | (next_byte(cs) & 0x3F);
    if (c < 0x000F0000) {
        return c & 0x0000FFFF; // (1 << 16) - 1
    }
    // 4 bytes
    c = (c << 6) | (next_byte(cs) & 0x3F);
    if (c < 0x03E00000) {
        return c & 0x001FFFFF; // (1 << 21) - 1
    }
    cs->current -= 4; // undo reading the last 4 bytes
#else
    cs->current -= 1; // undo reading the last byte
#endif // PARSER_UTF8_ENABLED
    assert(false); // should not ever be reached on valid input
    return -1; // get stuck in an invalid state
}
