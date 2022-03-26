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

#include "glserver.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// server settings
#define RING_SIZE_ORDER 12
#define RING_PACKET_SIZE_ORDER 15

static int nofork;

static void child_process(recvr_context_t* rc,
                          void(*handle_child)(recvr_context_t*))
{
  ring_init(&rc->ring, RING_SIZE_ORDER, RING_PACKET_SIZE_ORDER);
  recvr_run_loop(rc);
  handle_child(rc);
  if (pthread_join(rc->recvr_th, NULL) != 0)
    LOGE("pthread_join failed\n");
  recvr_stop(rc);
  LOGI("client terminated\n");
}

static void recvr_server_start(recvr_context_t* rc, const char* server_addr,
                               void(*handle_child)(recvr_context_t*))
{
  struct gls_server* srv = tport_server_create(server_addr);
  if (!srv)
    exit(EXIT_FAILURE);

  int quit = 0;
  while (!quit) {
    rc->cnx = tport_server_wait_connection(srv);
    if (!rc->cnx)
      break;
    LOGI("new client\n");

    if (nofork) {
      free(srv);
      child_process(rc, handle_child);
      exit(EXIT_SUCCESS);
    } else
      switch (fork()) {
      case -1:
        LOGE("%s: fork failed: %s\n", __FUNCTION__, strerror(errno));
        break;
      case 0:
        free(srv);
        child_process(rc, handle_child);
        exit(EXIT_SUCCESS);
      default:
        tport_close(rc->cnx);
        // ... loop and wait for a new client
      }
  }
  free(srv);
}

// FIXME: could use server_addr -- worth it?
static void recvr_connection_start(recvr_context_t* rc,
                                   void(*handle_child)(recvr_context_t*))
{
  rc->cnx = tport_connection_create();
  child_process(rc, handle_child);
}

int main(int argc, char* argv[])
{
  int opt;
  char* my_transport = NULL;
  char* my_addr = NULL;

  if (GLS_VERSION & 1)
    LOGW("this is a development GLS protocol, "
         "make sure client and server match\n");

  while ((opt = getopt(argc, argv, "nt:s:h")) != -1) {
    switch (opt) {
    case 't':
      my_transport = optarg;
      break;
    case 's':
      my_addr = optarg;
      break;
    case 'n':
      nofork = 1;
      break;
    case 'h':
    default:
      printf("Usage: %s [-n] [-t transport] [-s address]\n", argv[0]);
      return EXIT_SUCCESS;
    }
  }

  { // let forked processes be reaped for us
    struct sigaction sa = {.sa_handler=SIG_IGN};
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
      perror("sigaction(SIGCHLD)");
      return EXIT_FAILURE;
    }
  }

  if (tport_select(my_transport) < 0) {
    LOGE("cannot select transport\n");
    return EXIT_FAILURE;
  }

  if (tport_has_server_create()) {
    if (tport_has_connection_create()) { // let's see when it happens
      LOGE("transport provides both server and connection mode\n");
      return EXIT_FAILURE;
    }
    recvr_server_start(&glsec_global.rc, my_addr, glserver_handle_packets);
  } else if (tport_has_connection_create()) {
    (void)my_addr;
    recvr_connection_start(&glsec_global.rc, glserver_handle_packets);
  } else {
    LOGE("transport provides neither server nor connection mode\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
