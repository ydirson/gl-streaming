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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  int opt;
  recvr_context_t rc = {};
  char* my_transport = NULL;
  char* my_addr = NULL;

  if (GLS_VERSION & 1)
    fprintf(stderr, "GLS WARNING: this is a development GLS protocol, "
            "make sure client and server match\n");

  while ((opt = getopt(argc, argv, "t:s:h")) != -1) {
    switch (opt) {
    case 't':
      my_transport = optarg;
      break;
    case 's':
      my_addr = optarg;
      break;
    case 'h':
    default:
      printf("Usage: %s [-t transport] [-s address]\n", argv[0]);
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
    fprintf(stderr, "GLS ERROR: cannot select transport\n");
    return EXIT_FAILURE;
  }

  glsec_global.rc = &rc;
  recvr_server_start(&rc, my_addr, glserver_handle_packets);

  return EXIT_SUCCESS;
}
