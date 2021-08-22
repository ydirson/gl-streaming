#pragma once
#include "fifo.h"
#include "gls_command.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
  char *buf;
  size_t size;
} gls_buffer_t;

typedef struct {
  gls_buffer_t tmp_buf; // in_buf
  gls_buffer_t out_buf;
  int has_data;
} gls_bufpool_t;

// Copy a send_data chunk from fifo packet buffer to `pool`'s tmp_buf
// (soon to buffer pool)
static inline int fifobuf_data_to_bufpool(gls_bufpool_t* pool, fifo_t* fifo, gls_command_t* buf)
{
  (void)fifo;
  gls_cmd_send_data_t *c = (gls_cmd_send_data_t *)buf;
  assert (pool->tmp_buf.size);
  if (c->cmd_size > pool->tmp_buf.size) {
    fprintf(stderr, "GLS ERROR: data too large for buffer (%u > %zu), dropping\n",
            c->cmd_size, pool->tmp_buf.size);
    return 0;
  }
  memcpy(pool->tmp_buf.buf, c->data, c->cmd_size - sizeof(gls_cmd_send_data_t));
  pool->has_data = 1;
  return 1;
}
