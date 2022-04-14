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

#define _GNU_SOURCE
#include "ring.h"

#include <stdlib.h>

static int ring_malloc(ring_t* ring, void* data)
{
  (void)data; assert(!data); ring->allocator_data = NULL; // unused
  size_t size = sizeof(struct ring_control) + ring->ring_size;
  ring->storage = malloc(size);
  return ring->storage ? 0 : -1;
}

static void ring_free(ring_t* ring)
{
  free(ring->storage);
  ring->storage = NULL;
}

static ring_allocator_t heap_allocator = {
  .alloc = ring_malloc,
  .free = ring_free,
};

int ring_init(ring_t* ring, int notif_fd,
              ring_allocator_t* allocator, void* allocator_data,
              unsigned int ring_size_order)
{
  ring->ring_size = 1 << ring_size_order;

  if (notif_fd == -1) {
    if (notifier_init(&ring->notifier) < 0) {
      LOGE("RING notifier init error: %s\n", strerror(errno));
      return -1;
    }
  } else {
    if (notifier_init_fromfd(&ring->notifier, notif_fd) < 0) {
      LOGE("RING notifier init_fromfd error: %s\n", strerror(errno));
      return -1;
    }
  }

  ring->allocator = allocator ? allocator : &heap_allocator;

  if (ring->allocator->alloc(ring, allocator_data) < 0) {
    LOGE("ring allocation failure: %s\n", strerror(errno));
    return -1;
  }
  ring->control = ring->storage;
  ring->elements = (char*)&(ring->control[1]); // memory just behind ring control

  ring->control->idx_reader = 0;
  ring->control->idx_writer = 0;
  return 0;
}

int ring_delete(ring_t* ring)
{
  ring->control->idx_reader = 0;
  ring->control->idx_writer = 0;
  ring->allocator->free(ring);
  ring->ring_size = 0;

  notifier_close(&ring->notifier);
  return 0;
}
