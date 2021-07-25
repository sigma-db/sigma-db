#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t sigma_char;

struct char_stream;

int cs_init(struct char_stream **cs, const char *buf, size_t buf_len);

sigma_char cs_next(struct char_stream *cs);

sigma_char cs_peek_next(struct char_stream *cs);

int cs_peek(struct char_stream *cs, size_t k, sigma_char *dst);

int cs_seek(struct char_stream *cs, int offset);

size_t cs_pos(struct char_stream *cs);

bool cs_end(struct char_stream *cs);
