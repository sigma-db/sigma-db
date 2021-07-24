#pragma once

#include <stdint.h>
#include <stdbool.h>

struct char_stream;

int cs_init(struct char_stream **cs, const char *buf, size_t buf_len);

int32_t cs_next(struct char_stream *cs);

int32_t cs_peek(struct char_stream *cs, size_t k);

int cs_seek(struct char_stream *cs, int offset);

size_t cs_pos(struct char_stream *cs);

bool cs_end(struct char_stream *cs);
