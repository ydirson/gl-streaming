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
#include <fcntl.h>
#include <poll.h>
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


ssize_t tport_read(int fd, void* buffer, size_t bufsize)
{
  ssize_t recv_size = recv(fd, buffer, bufsize, 0);
  if (recv_size < 0)
    LOGE("GLS ERROR: transport socket recv: %s\n", strerror(errno));
  else if (recv_size == 0)
    LOGI("GLS INFO: transport socket closed\n");

  return recv_size;
}

// read everything into a scratch buffer to discard data
static int discard_bytes(int fd, size_t size, void* scratch, size_t scratch_size)
{
  LOGW("GLS ERROR: discarding large packet (%zu bytes)\n", size);
  do {
    size_t recv_size = tport_read(fd, scratch, scratch_size);
    if (recv_size <= 0) {
      close(fd);
      return 0;
    }

    assert ((size_t)recv_size <= size);
    size -= recv_size;
  } while (size);
  return 1;
}


void recvr_setup(recvr_context_t* rc)
{
  int sockopt = 1;
  if (setsockopt(rc->sock_fd, SOL_TCP, TCP_NODELAY, &sockopt, sizeof(sockopt)) < 0) {
    LOGE("GLS ERROR: setsockopt(TCP_NODELAY) error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

static void* socket_to_fifo_loop(void* data)
{
  recvr_context_t* rc = data;
  recvr_setup(rc);

  enum {
    POLLFD_TRANSPORT,
  };
  struct pollfd pollfds[] = {
    [POLLFD_TRANSPORT] = {
      .fd = rc->sock_fd,
      .events = POLLIN
    },
  };

  while (1) {
    int ret = poll(pollfds, sizeof(pollfds) / sizeof(pollfds[0]), -1);
    if (ret < 0) {
      LOGE("GLS ERROR: FIFO poll failed: %s\n", strerror(errno));
      break;
    }

    if (pollfds[POLLFD_TRANSPORT].revents & POLLHUP) {
      LOGI("GLS INFO: TRANSPORT poll hangup\n");
      break;
    }
    if (pollfds[POLLFD_TRANSPORT].revents & POLLERR) {
      LOGE("GLS ERROR: TRANSPORT poll error\n");
      break;
    }
    if (pollfds[POLLFD_TRANSPORT].revents & POLLIN) {
      int ret = recvr_handle_packet(rc);
      if (ret < 0)
        exit(EXIT_FAILURE);
      if (ret > 0) {
        //LOGD("GLS DEBUG: recvr_handle_packet said stop\n");
        break;
      }
      pollfds[POLLFD_TRANSPORT].revents &= ~POLLIN;
    }
    if (pollfds[POLLFD_TRANSPORT].revents)
      LOGW("GLS WARNING: FIFO poll revents=0x%x\n", pollfds[POLLFD_TRANSPORT].revents);
  }

  // end of thread
  fifo_writer_close(&rc->fifo);
  return NULL;
}

ssize_t recvr_read(int fd, void* buffer, size_t size)
{
  char* current = buffer;
  size_t remaining = size;
  while (remaining) {
    ssize_t recv_size = tport_read(fd, current, remaining);
    if (recv_size == 0)
      return 0;
    if (recv_size < 0)
      break;
    remaining -= recv_size;
    current += recv_size;
  }
  return size;
}

int recvr_handle_packet(recvr_context_t* rc)
{
  char* pushptr = fifo_push_ptr_get(&rc->fifo);
  if (pushptr == NULL) {
    LOGW("GLS WARNING: FIFO full!\n");
    usleep(SLEEP_USEC);
    return 0;
  }

  // look at message header to know its size and decide what to do
  ssize_t recv_size = recvr_read(rc->sock_fd, pushptr, sizeof(gls_command_t));
  if (recv_size < 0) {
    close(rc->sock_fd);
    return -1;
  } else if (recv_size == 0) {
    close(rc->sock_fd);
    return 1; // EOF
  } else if (recv_size != sizeof(gls_command_t)) {
    // internal error: transport should handle that
    LOGE("GLS ERROR: short read %zu != %zu\n", recv_size, sizeof(gls_command_t));
    close(rc->sock_fd);
    return -1;
  }

  gls_command_t* c = (gls_command_t*)pushptr;
  assert(c->cmd_size >= sizeof(gls_command_t)); // minimum sanity
  size_t remaining = c->cmd_size - sizeof(gls_command_t);

  // setup `dest` to point to a proper buffer depending on packet size ...
  char* dest;
  if (c->cmd_size <= rc->fifo.fifo_packet_size) {
    dest = pushptr;
  } else {
    if (c->cmd != GLSC_SEND_DATA) {
      // only SEND_DATA packets may be larger than a fifo packet
      LOGE("GLS ERROR: received large packet, not a SEND_DATA\n");
      if (discard_bytes(rc->sock_fd, remaining, pushptr, rc->fifo.fifo_packet_size))
        return 0;
      else
        return -1;
    }

    dest = malloc(c->cmd_size);
    if (!dest) {
      LOGE("GLS ERROR: malloc failed: %s\n", strerror(errno));
      if (discard_bytes(rc->sock_fd, remaining, pushptr, rc->fifo.fifo_packet_size))
        return 0;
      else
        return -1;
    }
    // copy the header into the malloc'd area
    memcpy(dest, pushptr, sizeof(gls_command_t));
    // advertized malloc'd zone
    ((gls_cmd_send_data_t*)c)->dataptr = dest;
  }
  // ... and adjust it for the data already read
  dest += sizeof(gls_command_t);

  int endsession = 0;
  while (remaining) {
    ssize_t recv_size = recvr_read(rc->sock_fd, dest, remaining);
    if (recv_size < 0) {
      close(rc->sock_fd);
      endsession = -1;
      break;
    } else if (recv_size == 0) {
      close(rc->sock_fd);
      endsession = 1;
      break;
    }

    remaining -= recv_size;
    dest += recv_size;
  }

  if (endsession)
    return endsession;

  if (c->cmd_size <= rc->fifo.fifo_packet_size && c->cmd == GLSC_SEND_DATA) {
    gls_cmd_send_data_t* data = (gls_cmd_send_data_t*)pushptr;
    if (data->zero != 0) {
      LOGW("GLS WARNING: SEND_DATA with non-zero 'zero' field %lx, compensating\n", data->zero);
      data->zero = 0;
    }
  }

  fifo_push_ptr_next(&rc->fifo);
  return 0;
}

int tport_server_create(const char* listen_addr, uint16_t listen_port)
{
  int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (listen_fd < 0) {
    fprintf(stderr, "GLS ERROR: receiver socket open: %s\n", strerror(errno));
    return -1;
  }

  int sockopt = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
    LOGE("GLS ERROR: setsockopt(SO_REUSEADDR) failed: %s\n", strerror(errno));
    return -1;
  }

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(listen_port);
  sai.sin_addr.s_addr = inet_addr(listen_addr);
  if (bind(listen_fd, (struct sockaddr*)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: bind failed: %s\n", strerror(errno));
    return -1;
  }

  if (listen(listen_fd, 1) < 0) {
    LOGE("GLS ERROR: listen failed: %s\n", strerror(errno));
    return -1;
  }

  return listen_fd;
}

void recvr_server_start(recvr_context_t* rc, const char* listen_addr, uint16_t listen_port,
                        void(*handle_child)(recvr_context_t*))
{
  int listen_fd = tport_server_create(listen_addr, listen_port);
  if (listen_fd < 0)
    exit(EXIT_FAILURE);

  int quit = 0;
  while (!quit) {
    rc->peer.addrlen = sizeof(rc->peer.addr);
    rc->sock_fd = accept4(listen_fd, &rc->peer.addr, &rc->peer.addrlen, 0);
    if (rc->sock_fd < 0) {
      LOGE("GLS ERROR: server accept: %s\n", strerror(errno));
      break;
    }
    LOGI("GLS INFO: new client\n");

    switch (fork()) {
    case -1:
      LOGE("GLS ERROR: %s: fork failed: %s\n", __FUNCTION__, strerror(errno));
      break;
    case 0: {
        fifo_init(&rc->fifo, FIFO_SIZE_ORDER, FIFO_PACKET_SIZE_ORDER);
        if (fcntl(rc->sock_fd, F_SETFD, FD_CLOEXEC) < 0) {
          LOGE("GLS ERROR: fcntl(FD_CLOEXEC) failed: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        pthread_create(&rc->recvr_th, NULL, socket_to_fifo_loop, rc);
        pthread_setname_np(rc->recvr_th, "gls-recvr");
        handle_child(rc);
        if (pthread_join(rc->recvr_th, NULL) != 0)
          LOGE("GLS ERROR: pthread_join failed\n");
        return;
      }
    default:
      close(rc->sock_fd);
      // ... loop and wait for a new client
    }
  }
}

void recvr_client_start(recvr_context_t* rc, const char* connect_addr, uint16_t connect_port)
{
  fifo_init(&rc->fifo, FIFO_SIZE_ORDER, FIFO_PACKET_SIZE_ORDER);

  rc->sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (rc->sock_fd < 0) {
    fprintf(stderr, "GLS ERROR: receiver socket open: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(connect_port);
  sai.sin_addr.s_addr = inet_addr(connect_addr);
  if (connect(rc->sock_fd, (struct sockaddr*)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: connect failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  pthread_create(&rc->recvr_th, NULL, socket_to_fifo_loop, rc);
  pthread_setname_np(rc->recvr_th, "gls-recvr");
}

void recvr_stop(recvr_context_t* rc)
{
  // FIXME should rather signal to socket_to_fifo_loop to finish
  // properly, feat. timeout-enabled poll() rather than blocking in
  // recv()
  pthread_cancel(rc->recvr_th);
  close(rc->sock_fd);
  fifo_delete(&rc->fifo);
}
