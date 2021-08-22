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
  gls_cmd_send_data_t *c = (gls_cmd_send_data_t *)buf;
  assert (dst->size);
  if (c->offset + c->size > dst->size) {
    fprintf(stderr, "GLS ERROR: data too large for buffer, dropping chunk with offset %d\n",
            c->offset);
    return c->isLast;
  }
  if (c->size > fifo->fifo_packet_size) {
    fprintf(stderr, "GLS ERROR: DATA packet size %u > fifo_packet_size %u\n",
            c->size, fifo->fifo_packet_size);
    return c->isLast;
  }
  memcpy(&dst->buf[c->offset], c->data.data_char, c->size);
  // FIXME: does not behave properly on out-of-order chunks
  return c->isLast;
}
