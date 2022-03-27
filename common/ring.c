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

int ring_init(ring_t* ring, unsigned int ring_size_order,
              unsigned int ring_packet_size_order)
{
  ring->ring_size = 1 << ring_size_order;
  ring->ring_packet_size = 1 << ring_packet_size_order;

  if (notifier_init(&ring->notifier) < 0) {
    LOGE("RING notifier init error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

#if 0
  // aligned malloc ?  Do we gain anything ?
  unsigned int alignment = ring->ring_packet_size;
  ring->buffer = (char*)aligned_alloc(alignment, ring->ring_size * ring->ring_packet_size);
  if (ring->buffer == NULL) {
    LOGE("ring allocation failure: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
#else
  ring->buffer = (char*)malloc(ring->ring_size * ring->ring_packet_size);
#endif

  ring->idx_reader = 0;
  ring->idx_writer = 0;
  return 0;
}

int ring_delete(ring_t* ring)
{
  free(ring->buffer);
  ring->buffer = NULL;
  ring->idx_reader = 0;
  ring->idx_writer = 0;
  ring->ring_size = 0;
  ring->ring_packet_size = 0;

  notifier_close(&ring->notifier);
  return 0;
}
