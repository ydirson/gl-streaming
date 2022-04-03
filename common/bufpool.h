#pragma once
#include "ring.h"
#include "gls_command.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char* buf;
  size_t size;
} gls_buffer_t;

typedef struct
{
  gls_buffer_t tmp_buf; // in_buf
  int has_data;
  char* mallocated;
  void* data_payload;
} gls_bufpool_t;

// Copy a send_data chunk from ring packet buffer to `pool`'s tmp_buf
static inline int ringbuf_data_to_bufpool(gls_bufpool_t* pool, ring_t* ring, gls_command_t* buf)
{
  (void)ring;
  if (pool->mallocated) {
    LOGW("previous buffer for large data not freed!\n");
    free(pool->mallocated);
    pool->mallocated = NULL;
  }
  gls_cmd_send_data_t* c = (gls_cmd_send_data_t*)buf;
  if (c->dataptr) {
    // large malloc'd zone, copy pointer not data
    pool->mallocated = c->dataptr; // transfer ownership
    pool->data_payload = pool->mallocated + sizeof(gls_cmd_send_data_t);
  } else {
    if (c->cmd_size > pool->tmp_buf.size) {
      LOGE("data too large for buffer (%u > %zu), dropping\n",
           c->cmd_size, pool->tmp_buf.size);
      return 0;
    }
    memcpy(pool->tmp_buf.buf, c->data, c->cmd_size - sizeof(gls_cmd_send_data_t));
  }
  pool->has_data = 1;
  return 1;
}
