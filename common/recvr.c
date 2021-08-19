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
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "recvr.h"
#include "fastlog.h"

#define TRUE 1
#define FALSE 0


static void* recvr_thread(recvr_context_t* rc)
{
  int quit = FALSE;

  while (quit == FALSE) {
    char* pushptr = fifo_push_ptr_get(&rc->fifo);
    if (pushptr == NULL) {
      LOGW("FIFO full!\n");
      usleep(rc->sleep_usec);
    } else {
      struct sockaddr addr;
      socklen_t addrlen = sizeof(addr);

      int recv_size = recvfrom(rc->sock_fd, pushptr, rc->fifo.fifo_packet_size, 0,
                               &addr, &addrlen);
      if (recv_size < 0) {
        LOGE("GLS ERROR: receiver socket recvfrom: %s\n", strerror(errno));
        quit = TRUE;
      }
      do {
        if (rc->peer.addrlen) {
          if (rc->peer.addrlen == addrlen && memcmp(&rc->peer.addr, &addr, addrlen) == 0)
            break;
          LOGW("GLS WARNING: received packet from different address\n");
        }
        memcpy(&rc->peer.addr, &addr, addrlen);
        rc->peer.addrlen = addrlen;
      } while(0);

      // FIXME check fifo packet overflow
      fifo_push_ptr_next(&rc->fifo);
    }
  }

  pthread_exit(NULL);
}


void recvr_init(recvr_context_t *rc)
{
  fifo_init(&rc->fifo, FIFO_SIZE_IN_BITS, FIFO_PACKET_SIZE_IN_BITS);
  rc->sleep_usec = SLEEP_USEC;

  rc->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (rc->sock_fd < 0) {
    fprintf(stderr, "GLS ERROR: receiver socket open: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void recvr_start(recvr_context_t *rc)
{
  pthread_create(&rc->recvr_th, NULL, (void* (*)(void*))recvr_thread, rc);
  pthread_setname_np(rc->recvr_th, "gls-recvr");
}


void recvr_stop_deinit(recvr_context_t *rc)
{
  pthread_cancel(rc->recvr_th);
  close(rc->sock_fd);
  fifo_delete(&rc->fifo);
}
