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
#include "notifier.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct ring ring_t;

typedef struct
{
  int (*alloc)(ring_t*, void*);
  void (*free)(ring_t*);
} ring_allocator_t;

struct ring_control
{
  unsigned idx_reader, idx_writer; // packet numbers
};

struct ring
{
  void* storage; // the whole including ring_control and buffer
  char* elements;
  struct ring_control* control;
  ring_allocator_t* allocator;
  void* allocator_data; // data that alloc() stores for free()
  unsigned int ring_size;
  struct notifier notifier;
};

// ring sizing - FIXME randomly chosen, should be more dynamic

#define CLT2SRV_API_RING_SIZE_ORDER 27
#define SRV2CLT_API_RING_SIZE_ORDER 12
#define CMD_RING_SIZE_ORDER 10

/*
 * ring_*_ptr_next() are not supposed to be called if the matching
 * ring_*_ptr_get() returns NULL, and will assert.
 */

static inline int ring_is_empty(ring_t* ring)
{
  return ring->control->idx_reader == ring->control->idx_writer;
}

// static inline size_t ring_freespace(ring_t* ring)
// {
//   size_t occupied = ((ring->ring_size + ring->control->idx_writer - ring->control->idx_reader) &
//                      (ring->ring_size - 1));
//   return ring->ring_size - occupied - 1;
// }

static inline size_t ring_contigfreespace(ring_t* ring)
{
  if (ring->control->idx_writer >= ring->control->idx_reader)
    return ring->ring_size - ring->control->idx_writer - 1;
  else
    return ring->control->idx_reader - ring->control->idx_writer - 1;
}

static inline char* ring_push_ptr_get(ring_t* ring)
{
  int next_idx = (ring->control->idx_writer + 1) & (ring->ring_size - 1);
  if (next_idx == ring->control->idx_reader)
    return NULL;
  return ring->elements + ring->control->idx_writer;
}

static inline void ring_push_ptr_next(ring_t* ring)
{
  int next_idx = (ring->control->idx_writer + 1) & (ring->ring_size - 1);
  assert (next_idx != ring->control->idx_reader);
  ring->control->idx_writer = next_idx;
  if (notify(&ring->notifier) < 0)
    LOGE("RING notifier write error: %s\n", strerror(errno));
}

static inline char* ring_pop_ptr_get(ring_t* ring)
{
  if (ring_is_empty(ring))
    return NULL;
  return ring->elements + ring->control->idx_reader;
}

static inline void ring_pop_ptr_next(ring_t* ring)
{
  assert (!ring_is_empty(ring));
  int next_idx = (ring->control->idx_reader + 1) & (ring->ring_size - 1);
  ring->control->idx_reader = next_idx;
}

static inline void ring_writer_close(ring_t* ring)
{
  // signal end of data to reader
  if (notifier_terminate(&ring->notifier) < 0)
    LOGE("RING notifier terminate error: %s\n", strerror(errno));
}


int ring_init(ring_t* ring, int notif_fd,
              ring_allocator_t* allocator, void* allocator_data,
              unsigned int ring_size_order);
int ring_delete(ring_t* ring);
