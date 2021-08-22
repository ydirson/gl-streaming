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
#include "bufpool.h"
#include "gls_command.h"
#include "glcontrol.h"
#include "recvr.h"
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define GLSE_TMP_BUFFER_SIZE 2097152
#define GLSE_OUT_BUFFER_SIZE 4096 // 2048

#define GLSE_SET_COMMAND_PTR(PTR, FUNCNAME)             \
  gls_##FUNCNAME##_t *PTR = (gls_##FUNCNAME##_t *)buf;  \
  //

#define GLSE_SET_RAWDATA_PTR(PTR, TYPE, WAITDATA)                       \
  TYPE* PTR;                                                            \
  if (WAITDATA) {                                                       \
    if (!glsec_global.pool.has_data) {                                  \
      fprintf(stderr, "GLS WARNING: skipping %s call without data\n", __FUNCTION__); \
      return;                                                           \
    }                                                                   \
    if (glsec_global.pool.mallocated)                                   \
      PTR = glsec_global.pool.data_payload;                             \
    else                                                                \
      PTR = (TYPE*)glsec_global.pool.tmp_buf.buf;                       \
  } else {                                                              \
    PTR = NULL;                                                         \
  }                                                                     \
  //
#define GLSE_RELEASE_DATA()                                             \
  glsec_global.pool.has_data = 0;                                       \
  if (glsec_global.pool.mallocated) {                                   \
    free(glsec_global.pool.mallocated);                                 \
    glsec_global.pool.mallocated = NULL;                                \
  }                                                                     \
  //
#define GLSE_SET_DATA_PTR(PTR, FUNCNAME, WAITDATA)              \
  GLSE_SET_RAWDATA_PTR(PTR, gls_data_##FUNCNAME##_t, WAITDATA)  \
  //

#define GLSE_SET_RAWRET_PTR(PTR, TYPE)                          \
  TYPE* ret = (TYPE*)glsec_global.pool.tmp_buf.buf;             \
  //
#define GLSE_SET_RET_PTR(PTR, FUNCNAME)                         \
  GLSE_SET_RAWRET_PTR(PTR, gls_ret_##FUNCNAME##_t);             \
  ret->cmd = GLSC_##FUNCNAME;                                   \
  //
#define GLSE_SEND_RAWRET(PTR, SIZE)                             \
  glse_cmd_send_data(SIZE, PTR);                                \
  //
#define GLSE_SEND_RET(PTR, FUNCNAME)                            \
  GLSE_SEND_RAWRET(PTR, sizeof(gls_ret_##FUNCNAME##_t));        \
  //


typedef struct
{
  gls_bufpool_t pool;
  recvr_context_t* rc;
  graphics_context_t *gc;
} glse_context_t;

extern glse_context_t glsec_global;
void glserver_handle_packets(recvr_context_t* rc);

int glse_cmd_send_data(uint32_t size, void *data);

int egl_executeCommand(gls_command_t *c);
int gles_executeCommand(gls_command_t *c);
