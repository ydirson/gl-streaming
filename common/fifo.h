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
#include <assert.h>
#include <stddef.h>

typedef struct
{
  char *buffer;
  int idx_reader, idx_writer; // packet numbers
  unsigned int fifo_size;
  unsigned int fifo_packet_size;
} fifo_t;

/*
 * fifo_*_ptr_next() are not supposed to be called if the matching
 * fifo_*_ptr_get() returns NULL, and will assert.
 */

static inline char* fifo_push_ptr_get(fifo_t *fifo)
{
  int next_idx = (fifo->idx_writer + 1) & (fifo->fifo_size - 1);
  if (next_idx == fifo->idx_reader)
    return NULL;
  return fifo->buffer + (fifo->idx_writer * fifo->fifo_packet_size);
}

static inline void fifo_push_ptr_next(fifo_t *fifo)
{
  int next_idx = (fifo->idx_writer + 1) & (fifo->fifo_size - 1);
  assert (next_idx != fifo->idx_reader);
  fifo->idx_writer = next_idx;
}

static inline char* fifo_pop_ptr_get(fifo_t *fifo)
{
  if (fifo->idx_reader == fifo->idx_writer)
    return NULL;
  return fifo->buffer + (fifo->idx_reader * fifo->fifo_packet_size);
}

static inline void fifo_pop_ptr_next(fifo_t *fifo)
{
  assert (fifo->idx_reader != fifo->idx_writer);
  int next_idx = (fifo->idx_reader + 1) & (fifo->fifo_size - 1);
  fifo->idx_reader = next_idx;
}


int fifo_init(fifo_t *fifo, unsigned int fifo_size_order,
              unsigned int fifo_packet_size_order);
int fifo_delete(fifo_t *fifo);
