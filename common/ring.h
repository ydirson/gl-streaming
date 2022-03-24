/*
Copyright (c) 2013, Shodruky Rhyammer
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include "fastlog.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
  char* buffer;
  int idx_reader, idx_writer; // packet numbers
  unsigned int ring_size;
  unsigned int ring_packet_size;
  int pipe_wr, pipe_rd;         // pipe for mainloop polling
} ring_t;

/*
 * ring_*_ptr_next() are not supposed to be called if the matching
 * ring_*_ptr_get() returns NULL, and will assert.
 */

static inline int ring_is_empty(ring_t* ring)
{
  return ring->idx_reader == ring->idx_writer;
}

static inline char* ring_push_ptr_get(ring_t* ring)
{
  int next_idx = (ring->idx_writer + 1) & (ring->ring_size - 1);
  if (next_idx == ring->idx_reader)
    return NULL;
  return ring->buffer + (ring->idx_writer * ring->ring_packet_size);
}

static inline void ring_push_ptr_next(ring_t* ring)
{
  int next_idx = (ring->idx_writer + 1) & (ring->ring_size - 1);
  assert (next_idx != ring->idx_reader);
  ring->idx_writer = next_idx;
  if (write(ring->pipe_wr, "", 1) < 0)
    LOGE("ring write to notification pipe: %s\n", strerror(errno));
}

static inline char* ring_pop_ptr_get(ring_t* ring)
{
  if (ring_is_empty(ring))
    return NULL;
  return ring->buffer + (ring->idx_reader * ring->ring_packet_size);
}

static inline void ring_pop_ptr_next(ring_t* ring)
{
  assert (!ring_is_empty(ring));
  char buf;
  if (read(ring->pipe_rd, &buf, 1) < 0)
    LOGE("ring read from notification pipe: %s\n", strerror(errno));
  int next_idx = (ring->idx_reader + 1) & (ring->ring_size - 1);
  ring->idx_reader = next_idx;
}

static inline void ring_writer_close(ring_t* ring)
{
  // signal end of data to reader
  if (close(ring->pipe_wr) < 0)
    LOGE("ring close of notification pipe: %s\n", strerror(errno));
}


int ring_init(ring_t* ring, unsigned int ring_size_order,
              unsigned int ring_packet_size_order);
int ring_delete(ring_t* ring);
