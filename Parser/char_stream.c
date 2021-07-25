#include <stdlib.h>

#include "char_stream.h"

#ifdef _NDEBUG
#define DISABLE_ASSERTIONS
#endif // !_NDEBUG

#include "../Common/assert.h"

extern int error;

#define assert_has_more(cs, on_failure) assertion(!cs_end(cs), default, , custom, on_failure)

struct char_stream {
    const char *buf;
    size_t buf_len;
    size_t pos;
};

int cs_init(struct char_stream **cs, const char *buf, size_t buf_sz)
{
    *cs = malloc(sizeof(struct char_stream));
    assert(*cs != NULL, custom, return -1);
    //if (*cs == NULL) {
    //    return -1;
    //}
    **cs = (struct char_stream){
        .buf = buf,
        .buf_len = buf_sz,
        .pos = 0
    };
    return 0;
}

sigma_char cs_next(struct char_stream *cs)
{
    // 1 byte
    assert_has_more(cs, return -1);
    sigma_char c = cs->buf[cs->pos++];
    if (c < 0x80) {
        return c;
    }
    // 2 bytes
    assert_has_more(cs, return -1);
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x00003800) {
        return c & 0x000007FF; // (1 << 11) - 1
    }
    // 3 bytes
    assert_has_more(cs, return -1);
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x000F0000) {
        return c & 0x0000FFFF; // (1 << 16) - 1
    }
    // 4 bytes
    assert_has_more(cs, return -1);
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x03E00000) {
        return c & 0x001FFFFF; // (1 << 21) - 1
    }
    // should not ever happen as we expect valid input
    return -1;
}

sigma_char cs_peek_next(struct char_stream *cs)
{
    assert_has_more(cs, return -1);
    size_t pos = cs->pos;
    sigma_char c = cs_next(cs);
    cs->pos = pos;
    return c;
}

int cs_peek(struct char_stream *cs, size_t k, sigma_char *dst)
{
    size_t pos = cs->pos;
    while (k-- > 0) {
        *(dst++) = cs_next(cs);
    }
    cs->pos = pos;
    return k == 0 ? 0 : -1;
}

int cs_peek_s(struct char_stream *cs, size_t k, sigma_char *dst)
{
    if (k == 0 || dst == NULL) {
        return -1;
    }
    int ret = 0;
    size_t pos = cs->pos;
    do {
        sigma_char c = cs_next(cs);
        if (c < 0) {
            ret = -1;
            goto exit_error;
        }
        *(dst++) = c;
    } while (--k > 0);
exit_error:
    cs->pos = pos;
    return ret;
}

int cs_seek(struct char_stream *cs, int offset)
{
    int dst = (int)cs->pos + offset;
    if (0 <= dst && dst < cs->buf_len) {
        cs->pos += dst;
        return 0;
    }
    return -1;
}

inline size_t cs_pos(struct char_stream *cs)
{
    return cs->pos;
}

inline bool cs_end(struct char_stream *cs)
{
    return cs->pos >= cs->buf_len;
}
