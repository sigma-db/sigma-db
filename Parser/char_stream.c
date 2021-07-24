#include <stdlib.h>

#include "char_stream.h"

struct char_stream {
    const char *buf;
    size_t buf_len;
    size_t pos;
};

int cs_init(struct char_stream **cs, const char *buf, size_t buf_len)
{
    *cs = malloc(sizeof(struct char_stream));
    if (*cs == NULL) {
        return EXIT_FAILURE;
    }
    (*cs)->buf = buf;
    (*cs)->buf_len = buf_len;
    (*cs)->pos = 0;
    return EXIT_SUCCESS;
}

int32_t cs_next(struct char_stream *cs)
{
    // 1 byte
    char c = cs->buf[cs->pos++];
    if (c < 0x80) {
        return c;
    }
    // 2 bytes
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x00003800) {
        return c & 0x000007FF; // (1 << 11) - 1
    }
    // 3 bytes
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x000F0000) {
        return c & 0x0000FFFF; // (1 << 16) - 1
    }
    // 4 bytes
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x03E00000) {
        return c & 0x001FFFFF; // (1 << 21) - 1
    }
    // should not ever happen as we expect valid input
    return -1;
}

int32_t cs_peek(struct char_stream *cs, size_t k)
{
    const size_t pos = cs->pos;
    const int32_t result = cs_next(cs);
    cs->pos = pos;
    return result;
}

int cs_seek(struct char_stream *cs, int offset)
{
    int dst = cs->pos + offset;
    if (0 <= dst && dst < cs->buf_len) {
        cs->pos += dst;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

inline size_t cs_pos(struct char_stream *cs)
{
    return cs->pos;
}

inline bool cs_end(struct char_stream *cs)
{
    return cs->pos < cs->buf_len;
}
