/**
 * Iterates a given character sequence perceived as a null-terminated string and emits one code point at a time.
 * If the charset in use is UNICODE, we assume the string to be UTF-8 encoded.
 */

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>

#define UNICODE
#define CHAR_STREAM_LOOKAHEAD 1 // NOTE: set return type of peek function accordingly

#ifdef UNICODE
#   define CHAR_TYPE char32_t
#else
#   define CHAR_TYPE char
#endif // UNICODE

typedef CHAR_TYPE sigma_char_t;

struct char_stream;

// `buf` is a null-terminated string of at most bufsz chars including the terminating \0 char
// For bufsz = 0, internal bounds-checking is deactivated (beware of buffer overflows)
int char_stream_create(struct char_stream **cs, const char *buf, size_t bufsz);

// Releases internal resources but not the string itself
void char_stream_destroy(struct char_stream *cs);

// Returns the next code point from the buffer or -1 when an error occurred during decoding
sigma_char_t char_stream_next(struct char_stream *cs);

// Retrieves the k-th next character (if available) without forwarding the stream or -1 when an error occurred during decoding
sigma_char_t char_stream_peek(struct char_stream *cs, size_t k);

// Forwards the stream by `delta` code points and returns the enxt code point after forwarding
// 
// Note that advancing past the terminating null-character (or even past the underlying buffer's end) 
// will eventually lead to errors on subsequent memory accesses for strings of unspecified size (i.e. with bufsz = 0)
size_t char_stream_advance(struct char_stream *cs, size_t delta);

// Current (byte) offset from the beginning
size_t char_stream_pos(struct char_stream *cs);

bool char_stream_has_more(struct char_stream *cs);

#endif // !CHAR_STREAM_H
