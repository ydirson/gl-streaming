#pragma once
#include "fifo.h"
#include "gls_command.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GLS_TMP_BUFFER_SIZE 2097152
#define GLS_ALIGNMENT_BITS 3

typedef struct
{
  char *buf;
  size_t size;
  uint32_t ptr; // for accumulation, only for batching into tmp_buf
} gls_buffer_t;


// for batching too
static inline uint32_t next_ptr(uint32_t ptr, uint32_t size, uint32_t alignment_bits)
{
  uint32_t a = (1 << alignment_bits) - 1;
  return (ptr + size + a) & ~a;
}

// Copy a send_data chunk from fifo packet buffer to `dst` buffer
// (soon to buffer pool)
static inline int fifobuf_data_to_bufpool(gls_buffer_t* dst, fifo_t* fifo, gls_command_t* buf)
{
  (void)fifo;
  gls_cmd_send_data_t *c = (gls_cmd_send_data_t *)buf;
  assert (dst->size);
  if (c->cmd_size > dst->size) {
    fprintf(stderr, "GLS ERROR: data too large for buffer (%u > %zu), dropping\n",
            c->cmd_size, dst->size);
    return 0;
  }
  memcpy(dst->buf, c->data, c->cmd_size - sizeof(gls_cmd_send_data_t));
  return 1;
}
