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

#include "fifo.h"

#include <unistd.h>
#include <arpa/inet.h>

#define SLEEP_USEC 1000
#define FIFO_SIZE_ORDER 6
#define FIFO_PACKET_SIZE_ORDER 19

typedef struct
{
  pthread_t recvr_th;

  fifo_t fifo;
  useconds_t sleep_usec;

  struct {
    struct sockaddr addr;
    socklen_t addrlen;
  } peer;

  // On server side, sock_fd is first used for the listening socket,
  // until we start the accept loop, where it takes the communication
  // FD as on client side.  FIXME: maybe avoid that juggling.
  int sock_fd;
} recvr_context_t;

void recvr_server_start(recvr_context_t *rc, const char* listen_addr, uint16_t listen_port);
void recvr_client_start(recvr_context_t *rc, const char* connect_addr, uint16_t connect_port);
void recvr_stop(recvr_context_t *c);