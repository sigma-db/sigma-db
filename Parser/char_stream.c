#include <stdlib.h>

#include "char_stream.h"

#ifdef PARSER_CHECK_BOUNDS
#   define ENABLE_ASSERTIONS_IN_RELEASE 1
#endif // !PARSER_CHECK_BOUNDS

#include "../Common/assert.h"

extern int error;

#define assert_has_more(cs) \
    assertion(!cs_end(cs), custom, "No more characters in char_stream " #cs ".")

struct char_stream {
    const char *buf;
    size_t buf_len;
    size_t pos;
};

int cs_init(struct char_stream **cs, const char *buf, size_t buf_sz)
{
    *cs = malloc(sizeof(struct char_stream));
    assert_not_null(*cs, return, -1);
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
    assert_has_more(cs);
    sigma_char c = cs->buf[cs->pos++];
    if (c < 0x80) {
        return c;
    }
    // 2 bytes
    assert_has_more(cs);
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x00003800) {
        return c & 0x000007FF; // (1 << 11) - 1
    }
    // 3 bytes
    assert_has_more(cs);
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x000F0000) {
        return c & 0x0000FFFF; // (1 << 16) - 1
    }
    // 4 bytes
    assert_has_more(cs);
    c = (c << 6) | (cs->buf[cs->pos++] & 0x3F);
    if (c < 0x03E00000) {
        return c & 0x001FFFFF; // (1 << 21) - 1
    }
    // should not ever happen as we expect valid input
    return -1;
}

sigma_char cs_peek_next(struct char_stream *cs)
{
    assert_has_more(cs);
    size_t pos = cs->pos;
    sigma_char c = cs_next(cs);
    cs->pos = pos;
    return c;
}

int cs_peek(struct char_stream *cs, size_t k, sigma_char *dst)
{
    assert_not_null(dst);
    int ret = 0;
    size_t pos = cs->pos;
    while (k-- > 0) {
        assert_has_more(cs);
        *(dst++) = cs_next(cs);
    }
    cs->pos = pos;
    return k == 0 ? 0 : -1;
}

int cs_seek(struct char_stream *cs, int offset)
{
    int dst = (int)cs->pos + offset;
    assert_within_bounds(0, dst, cs->buf_len);
    cs->pos += dst;
    return 0;
}

inline size_t cs_pos(struct char_stream *cs)
{
    return cs->pos;
}

inline bool cs_end(struct char_stream *cs)
{
    return cs->pos >= cs->buf_len;
}
