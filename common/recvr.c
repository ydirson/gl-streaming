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

#include "recvr.h"
#include "gls_command.h"
#include "fastlog.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#ifdef GLS_SERVER
# define FIFO_SIZE_ORDER 12
# define FIFO_PACKET_SIZE_ORDER 15
#else
# define FIFO_SIZE_ORDER 2
# define FIFO_PACKET_SIZE_ORDER 10
#endif

static void recvr_init(recvr_context_t *rc)
{
  fifo_init(&rc->fifo, FIFO_SIZE_ORDER, FIFO_PACKET_SIZE_ORDER);
  rc->sleep_usec = SLEEP_USEC;

  rc->sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (rc->sock_fd < 0) {
    fprintf(stderr, "GLS ERROR: receiver socket open: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}


// read everything into a scratch buffer to discard data
static int discard_bytes(int fd, size_t size, void* scratch, size_t scratch_size)
{
  LOGW("GLS ERROR: discarding large packet (%zu bytes)\n", size);
  do {
    int recv_size = recv(fd, scratch, scratch_size, 0);
    if (recv_size < 0) {
      LOGE("GLS ERROR: receiver socket recv: %s\n", strerror(errno));
      close(fd);
      return 0;
    } else if (recv_size == 0) {
      LOGI("GLS INFO: connection closed\n\n");
      close(fd);
      return 0;
    }

    assert ((unsigned)recv_size <= size);
    size -= recv_size;
  } while(size);
  return 1;
}


static void socket_to_fifo_loop(recvr_context_t* rc)
{
  {
    int sockopt = 1;
    if (setsockopt(rc->sock_fd, SOL_TCP, TCP_NODELAY, &sockopt, sizeof(sockopt)) < 0) {
      fprintf(stderr, "GLS ERROR: setsockopt(TCP_NODELAY) error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  int endsession = 0;
  while (!endsession) {
    char* pushptr = fifo_push_ptr_get(&rc->fifo);
    if (pushptr == NULL) {
      LOGW("GLS WARNING: FIFO full!\n");
      usleep(rc->sleep_usec);
      continue;
    }

    // look at message header to know its size and decide what to do
    int recv_size = recv(rc->sock_fd, pushptr, sizeof(gls_command_t), MSG_PEEK | MSG_WAITALL);
    if (recv_size < 0) {
      LOGE("GLS ERROR: receiver socket recv: %s\n", strerror(errno));
      close(rc->sock_fd);
      break;
    } else if (recv_size == 0) {
      LOGI("GLS INFO: connection closed\n\n");
      close(rc->sock_fd);
      break;
    } else if (recv_size != sizeof(gls_command_t)) {
      LOGE("GLS ERROR: receiver socket recv: requested %zu bytes, read %d\n",
           sizeof(gls_command_t), recv_size);
      close(rc->sock_fd);
      break;
    }

    gls_command_t* c = (gls_command_t*)pushptr;
    assert(c->cmd_size >= sizeof(gls_command_t)); // minimum sanity

    char* dest;
    size_t remaining = c->cmd_size;
    if (c->cmd_size <= rc->fifo.fifo_packet_size) {
      dest = pushptr;
    } else {
      if (c->cmd != GLSC_SEND_DATA) {
        // only SEND_DATA packets may be larger than a fifo packet
        LOGE("GLS ERROR: received large packet, not a SEND_DATA\n");
        if (discard_bytes(rc->sock_fd, remaining, pushptr, rc->fifo.fifo_packet_size))
          continue;
        else
          break;
      }

      dest = malloc(c->cmd_size);
      if (!dest) {
        LOGE("GLS ERROR: malloc failed: %s\n", strerror(errno));
        if (discard_bytes(rc->sock_fd, remaining, pushptr, rc->fifo.fifo_packet_size))
          continue;
        else
          break;
      }
      // advertized malloc'd zone
      ((gls_cmd_send_data_t*)c)->dataptr = dest;
    }

    do {
      recv_size = recv(rc->sock_fd, dest, remaining, 0);
      if (recv_size < 0) {
        LOGE("GLS ERROR: receiver socket recv: %s\n", strerror(errno));
        close(rc->sock_fd);
        break;
      } else if (recv_size == 0) {
        LOGI("GLS INFO: connection closed\n\n");
        close(rc->sock_fd);
        break;
      }

      remaining -= recv_size;
      dest += recv_size;
    } while(remaining);

    if (c->cmd_size <= rc->fifo.fifo_packet_size && c->cmd == GLSC_SEND_DATA) {
      gls_cmd_send_data_t* data = (gls_cmd_send_data_t*)pushptr;
      if (data->zero != 0) {
        LOGW("GLS WARNING: SEND_DATA with non-zero 'zero' field %lx, compensating\n", data->zero);
        data->zero = 0;
      }
    }

    fifo_push_ptr_next(&rc->fifo);
  }
}

static void* recvr_server_thread(void* data)
{
  recvr_context_t* rc = (recvr_context_t*)data;
  int listen_fd = rc->sock_fd;
  int quit = 0;
  while (!quit) {
    rc->peer.addrlen = sizeof(rc->peer.addr);
    rc->sock_fd = accept4(listen_fd, &rc->peer.addr, &rc->peer.addrlen, SOCK_CLOEXEC);
    if (rc->sock_fd < 0) {
      LOGE("GLS ERROR: server accept: %s\n", strerror(errno));
      break;
    }

    socket_to_fifo_loop(rc);
  }

  return NULL;
}

static void* recvr_client_thread(void* data)
{
  recvr_context_t* rc = (recvr_context_t*)data;
  socket_to_fifo_loop(rc);
  return NULL;
}


void recvr_server_start(recvr_context_t *rc, const char* listen_addr, uint16_t listen_port)
{
  recvr_init(rc);

  int sockopt = 1;
  if (setsockopt(rc->sock_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
    LOGE("GLS ERROR: setsockopt(SO_REUSEADDR) failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(listen_port);
  sai.sin_addr.s_addr = inet_addr(listen_addr);
  if (bind(rc->sock_fd, (struct sockaddr *)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: bind failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (listen(rc->sock_fd, 1) < 0) {
    LOGE("GLS ERROR: listen failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  pthread_create(&rc->recvr_th, NULL, recvr_server_thread, rc);
  pthread_setname_np(rc->recvr_th, "gls-recvr");
}

void recvr_client_start(recvr_context_t *rc, const char* connect_addr, uint16_t connect_port)
{
  recvr_init(rc);

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(connect_port);
  sai.sin_addr.s_addr = inet_addr(connect_addr);
  if (connect(rc->sock_fd, (struct sockaddr *)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: connect failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  pthread_create(&rc->recvr_th, NULL, (void* (*)(void*))recvr_client_thread, rc);
  pthread_setname_np(rc->recvr_th, "gls-recvr");
}

void recvr_stop(recvr_context_t *rc)
{
  pthread_cancel(rc->recvr_th);
  close(rc->sock_fd);
  fifo_delete(&rc->fifo);
}
