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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  int opt;
  recvr_context_t rc = {};

  if (GLS_VERSION & 1)
    fprintf(stderr, "WARNING: this is a development GLS protocol, "
            "make sure client and server match\n");

  char my_ip[255];
  uint16_t my_port = 18145;
  strcpy(my_ip, "127.0.0.1");
  while ((opt = getopt(argc, argv, "s:c:h")) != -1) {
    switch (opt) {
    case 's':
      strncpy(my_ip, strtok(optarg, ":"), sizeof(my_ip) - 1);
      my_port = atoi(strtok(NULL, ":"));
      break;
    case 'h':
    default:
      printf("Usage: %s [-s my_ip_address:port]\n", argv[0]);
      return 0;
    }
  }
#ifdef USE_X11
  // FIXME: this is hardcoded in several places
  glsurfaceview_width = 1280;
  glsurfaceview_height = 720;
#endif

  recvr_server_start(&rc, my_ip, my_port);
  glserver_handle_packets(&rc);
  recvr_stop(&rc);

  return 0;
}
