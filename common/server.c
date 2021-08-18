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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#include "server.h"
#include "fastlog.h"

#define TRUE 1
#define FALSE 0


void server_init(server_context_t *c)
{
  c->err = FALSE;
  c->fifo_packet_size_in_bits = FIFO_PACKET_SIZE_IN_BITS;
  c->fifo_size_in_bits = FIFO_SIZE_IN_BITS;
  c->sleep_usec = SLEEP_USEC;
}


void * server_thread(void * arg)
{
#ifdef GLS_SERVER // GLS_SERVER
  server_thread_args_t * a = (server_thread_args_t *)arg;
#else // GL_CLIENT
  server_context_t * a = (server_context_t *)arg;
#endif // GL_CLIENT
  int quit = FALSE;

  while (quit == FALSE) {
#ifdef GLS_SERVER // GLS_SERVER
    char* pushptr = fifo_push_ptr_get(a->fifo);
#else // GL_CLIENT
    char* pushptr = fifo_push_ptr_get(&a->fifo);
#endif // GL_CLIENT
    if (pushptr == NULL) {
      LOGW("FIFO full!\n");
      usleep(a->sleep_usec);
    } else {
      int recive_size = recvfrom(a->sock_fd, pushptr, a->max_packet_size, 0, NULL, NULL);
      if (recive_size == -1) {
        LOGE("Socket recvfrom Error.\n");
        quit = TRUE;
      }
#ifdef GLS_SERVER // GLS_SERVER
    fifo_push_ptr_next(a->fifo);
#else // GL_CLIENT
    fifo_push_ptr_next(&a->fifo);
#endif // GL_CLIENT
    }
  }

  pthread_exit(NULL);
}

void set_server_address_port(server_context_t *c, char * addr, uint16_t port)
{
#ifdef GLS_SERVER
	strncpy(c->server_thread_arg.addr, addr, sizeof(c->server_thread_arg.addr) - 1);
	c->server_thread_arg.port = port;
#else
	strncpy(c->addr, addr, sizeof(c->addr) - 1);
	c->port = port;
#endif
}


void set_client_address_port(server_context_t *c, char * addr, uint16_t port)
{
#ifdef GLS_SERVER
	strncpy(c->popper_thread_arg.addr, addr, sizeof(c->popper_thread_arg.addr) - 1);
	c->popper_thread_arg.port = port;
#else
	strncpy(c->bind_addr, addr, sizeof(c->bind_addr) - 1);
	c->bind_port = port;
#endif
}

// #endif // GL_CLIENT


void socket_open(server_context_t *c)
{
// There's nothing same to merge
#ifdef GLS_SERVER // GLS_SERVER
  c->server_thread_arg.sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (c->server_thread_arg.sock_fd == -1)
  {
    LOGE("Server Socket Open Error.");
    // exit(EXIT_FAILURE);
	
	return;
  }

  c->popper_thread_arg.sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (c->popper_thread_arg.sock_fd == -1)
  {
    LOGE("Client Socket Open Error.");
    // exit(EXIT_FAILURE);
	
	return;
  }

  c->server_thread_arg.sai.sin_family = AF_INET;
  c->server_thread_arg.sai.sin_port = htons(c->server_thread_arg.port);
  c->server_thread_arg.sai.sin_addr.s_addr = inet_addr(c->server_thread_arg.addr);

  c->popper_thread_arg.sai.sin_family = AF_INET;
  c->popper_thread_arg.sai.sin_port = htons(c->popper_thread_arg.port);
  c->popper_thread_arg.sai.sin_addr.s_addr = inet_addr(c->popper_thread_arg.addr);

  if (bind(c->server_thread_arg.sock_fd, (struct sockaddr *)&c->server_thread_arg.sai, sizeof(struct sockaddr_in)) == -1)
  {
    LOGE("Socket Bind Error.");
    // exit(EXIT_FAILURE);
	
	return;
  }
#else // GL_CLIENT
  struct sockaddr_in sai;
  c->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (c->sock_fd == -1)
  {
    printf("Server Socket Open Error.\n");
    exit(EXIT_FAILURE);
  }

  c->sai.sin_family = AF_INET;
  c->sai.sin_port = htons(c->port);
  c->sai.sin_addr.s_addr = inet_addr(c->addr);

  sai.sin_family = AF_INET;
  sai.sin_port = htons(c->bind_port);
  sai.sin_addr.s_addr = inet_addr(c->bind_addr);

  if (bind(c->sock_fd, (struct sockaddr *)&sai, sizeof(struct sockaddr_in)) == -1)
  {
    printf("Socket Bind Error.\n");
    exit(EXIT_FAILURE);
  }
#endif // GL_CLIENT
}


void socket_close(server_context_t *c)
{
#ifdef GLS_SERVER // GLS_SERVER
  close(c->server_thread_arg.sock_fd);
  close(c->popper_thread_arg.sock_fd);
#else // GL_CLIENT
  close(c->sock_fd);
#endif // GL_CLIENT
}


// #ifdef GLS_SERVER // GLS_SERVER
server_context_t *server_context;
void server_run(server_context_t *ctx, void *(*popper_thread)(void *))
{
  server_context_t *c;
  if (ctx == NULL) {
	  c = server_context;
  } else {
	  c = ctx;
	  server_context = ctx;
  }
  fifo_init(&c->fifo, c->fifo_size_in_bits, c->fifo_packet_size_in_bits);
  c->server_thread_arg.fifo = &c->fifo;
  c->popper_thread_arg.fifo = &c->fifo;
  c->server_thread_arg.max_packet_size = 1 << c->fifo_packet_size_in_bits;
  c->popper_thread_arg.max_packet_size = 1 << c->fifo_packet_size_in_bits;
  c->server_thread_arg.sleep_usec = c->sleep_usec;
  c->popper_thread_arg.sleep_usec = c->sleep_usec;
  socket_open(c);
  pthread_create(&c->server_th, NULL, (void* (*)(void*))server_thread, &c->server_thread_arg);
  pthread_setname_np(c->server_th, "gls-recver");

  popper_thread(&c->popper_thread_arg);
  
  pthread_cancel(c->server_th);
  
  socket_close(c);
  fifo_delete(&c->fifo);
}


#ifndef GLS_SERVER // GL_CLIENT
void *server_start(server_context_t *c)
{
  fifo_init(&c->fifo, c->fifo_size_in_bits, c->fifo_packet_size_in_bits);
  c->max_packet_size = 1 << c->fifo_packet_size_in_bits;

  socket_open(c);

  pthread_create(&c->server_th, NULL, (void* (*)(void*))server_thread, c);
  pthread_setname_np(c->server_th, "gls-recver");
  return c;
}


void server_stop(server_context_t *c)
{
    pthread_cancel(c->server_th);
    socket_close(c);
    fifo_delete(&c->fifo);
}
#endif // GL_CLIENT
