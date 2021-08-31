#include "test.h"

#include "../Parser/char_stream.h"

static struct char_stream *cs;

TEST(init)
{
    int error = char_stream_create(&cs, "David Luis Wiegandt", 0);
    EXPECT(!error && cs != NULL);
}

TEST(peek)
{
    sigma_char_t cp;

    cp = char_stream_peek(cs, 4);
    EXPECT(cp == 'i');

    char_stream_advance(cs, 7);
    cp = char_stream_next(cs);
    EXPECT(cp == 'u');

    cp = char_stream_peek(cs, 2);
    EXPECT(cp == 's');
}

TEST(iterate)
{
    for (size_t i = 8; char_stream_has_more(cs); i++) {
        size_t pos = char_stream_pos(cs);
        EXPECT(pos == i);

        sigma_char_t cp = char_stream_next(cs);
        EXPECT(cp >= 0);
    }
    EXPECT(!char_stream_has_more(cs));
}

// TODO: Add tests for UTF-8 strings with multi-byte characters

SUITE(test_char_stream)
{
    // TODO: Add macros BEFORE({ <code> }) and AFTER({ <code> }) that define code blocks executed before and after every test
    RUN(init, peek, iterate);
}
