#pragma once

#include <stdbool.h>

struct statement {

};

struct statement_stream;

int ss_init(struct statement_stream **ss, const char *buf, size_t buf_sz);

struct statement ss_next(struct statement_stream *ss);

struct statement ss_peek(struct statement_stream *ss);

bool ss_end(struct statement_stream *ss);
