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
#include <stdio.h>

#include "bufpool.h"
#include "recvr.h"
#include "gls_command.h"

typedef struct
{
  gls_buffer_t tmp_buf;
  gls_buffer_t out_buf;
  recvr_context_t rc;
  struct {
    struct sockaddr addr;
    socklen_t addrlen;
  } server;
  uint32_t screen_width;
  uint32_t screen_height;
  int32_t pack_alignment;
  int32_t unpack_alignment;
  int is_debug;
} gls_context_t;


#define GLS_OUT_BUFFER_SIZE 4096 // 2048
#define GLS_TIMEOUT_SEC 3.0f

// gls_glFunctionName_t *c = (gls_glFunctionName_t *)glsc_global.out_buf.buf;
// c->cmd = GLSC_glFunctionName;
// c->cmd = sizeof(gls_glFunctionName_t);
#define GLS_SET_COMMAND_PTR(PTR, FUNCNAME)                              \
  gls_##FUNCNAME##_t *PTR = (gls_##FUNCNAME##_t *)glsc_global.out_buf.buf; \
  PTR->cmd = GLSC_##FUNCNAME;                                           \
  PTR->cmd_size = sizeof(gls_##FUNCNAME##_t);                           \
  if (glsc_global.is_debug)                                             \
    fprintf(stderr, "gls debug: handling %s\n", #FUNCNAME);

// send_packet(sizeof(gls_glFunctionName_t));
#define GLS_SEND_PACKET(FUNCNAME) send_packet()

// aliases/stubs for potentially-batchable commands
#define GLS_SET_COMMAND_PTR_BATCH(PTR, FUNCNAME) GLS_SET_COMMAND_PTR(PTR, FUNCNAME)
#define GLS_PUSH_BATCH(FUNCNAME) GLS_SEND_PACKET(FUNCNAME)
#define push_batch_command(SIZE) send_packet(SIZE)
#define check_batch_overflow(SIZE,MSG) (TRUE)
#define gls_cmd_flush() do {} while(0)


#define WARN_STUBBED() do {                                             \
    static int shown = 0;                                               \
    if (!shown) {                                                       \
      fprintf(stderr, "WARNING: %s is stubbed\n", __FUNCTION__);        \
      shown = 1;                                                        \
    }                                                                   \
  } while(0);

#define TRUE 1
#define FALSE 0

extern gls_context_t glsc_global;
extern uint32_t client_egl_error;

void gls_init_library(void);
void gls_cleanup_library(void);

extern gls_context_t glsc_global;
int send_packet(void);
int gls_cmd_send_data(uint32_t size, const void *data);
int wait_for_data(char *str);
