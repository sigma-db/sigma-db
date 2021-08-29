/**
 * Iterates a given character sequence perceived as a null-terminated string and emits one code point at a time.
 * If the charset in use is UNICODE, we assume the string to be UTF-8 encoded.
 */

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>

#define PARSER_UTF8_ENABLED
#define CHAR_STREAM_LOOKAHEAD 1

#ifdef PARSER_UTF8_ENABLED
#   define CHAR_TYPE char32_t
#else
#   define CHAR_TYPE char
#endif

typedef CHAR_TYPE sigma_char_t;

struct char_stream;

// `buf` is a null-terminated string of at most bufsz chars including the terminating \0 char
// Set bufsz = 0 if the buffer's size is not known in advance (beware of buffer overflows)
int char_stream_create(struct char_stream **cs, const char *buf, size_t bufsz);

// Releases internal resources but not the string itself
void char_stream_destroy(struct char_stream *cs);

// Returns the next code point from the buffer
// The return value is only defined when char_stream_has_more(cs) == true
sigma_char_t char_stream_next(struct char_stream *cs);

// Returns the k-th next character (if available) without forwarding the stream
sigma_char_t char_stream_peek(struct char_stream *cs, size_t k);

// Forwards the stream by k code points
void char_stream_advance(struct char_stream *cs, size_t delta);

// Current byte offset from the beginning
size_t char_stream_pos(struct char_stream *cs);

bool char_stream_has_more(struct char_stream *cs);

#endif // !CHAR_STREAM_H
