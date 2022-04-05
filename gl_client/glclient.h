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
#include <arpa/inet.h>
#include <EGL/eglplatform.h>

#include "bufpool.h"
#include "xmitr.h"
#include "recvr.h"
#include "gls_command.h"

typedef struct
{
  gls_bufpool_t pool;
  struct xmitr* cmd_xmitr;
  struct xmitr* api_xmitr;
  recvr_context_t rc;
  struct
  {
    struct sockaddr addr;
    socklen_t addrlen;
  } server;
  int32_t pack_alignment;
  int32_t unpack_alignment;
  int is_debug;
} gls_context_t;

#define GLS_TMP_BUFFER_SIZE 2097152
#define GLS_OUT_BUFFER_SIZE 4096 // 2048

#define GLS_SET_COMMAND_PTR(PTR, FUNCNAME)                              \
  _GLS_SET_COMMAND_PTR(PTR, FUNCNAME, GLSC_##FUNCNAME)
#define _GLS_SET_COMMAND_PTR(PTR, FUNCNAME, CMD)                        \
  gls_##FUNCNAME##_t *PTR = (gls_##FUNCNAME##_t *)xmitr_getbuf(glsc_global.api_xmitr); \
  PTR->cmd = CMD;                                                       \
  PTR->cmd_size = sizeof(gls_##FUNCNAME##_t);                           \
  if (glsc_global.is_debug)                                             \
    LOGD("handling %s (cmd=0x%x)\n",                         \
         #FUNCNAME, CMD);                                               \
  //

#define GLS_ENOUGH_SIZE(PTR) (PTR->cmd_size <= GLS_OUT_BUFFER_SIZE)
#define GLS_VARIABLE_PAYLOAD(PTR, FIELD, SIZE)                          \
  _GLS_VARIABLE_PAYLOAD(PTR, FIELD, SIZE, return)
#define _GLS_VARIABLE_PAYLOAD(PTR, FIELD, SIZE, RETURN_STMT)            \
  do {                                                                  \
    PTR->cmd_size += SIZE;                                              \
    if (!GLS_ENOUGH_SIZE(PTR)) {                                        \
      LOGE("%s data too large\n", __FUNCTION__);             \
      RETURN_STMT;                                                      \
    }                                                                   \
    memcpy(PTR->FIELD, FIELD, SIZE);                                    \
  } while (0)                                                           \
  //

#define GLS_SEND_PACKET(FUNCNAME) send_packet(glsc_global.api_xmitr)
#define GLS_SEND_DATA(SIZE, DATA) gls_cmd_send_data(glsc_global.api_xmitr, SIZE, DATA)

#define GLS_WAIT_RET(TYPE, FUNCNAME)                            \
  wait_for_data(TYPE, #FUNCNAME);                               \
  //
#define GLS_SET_RAWRET_PTR(PTR, TYPE, FUNCNAME)                 \
  TYPE* PTR;                                                    \
  if (glsc_global.pool.mallocated)                              \
    PTR = glsc_global.pool.data_payload;                        \
  else                                                          \
    PTR = (TYPE*)glsc_global.pool.tmp_buf.buf;                  \
   //
#define GLS_SET_RET_PTR(PTR, FUNCNAME)                          \
  GLS_SET_RAWRET_PTR(PTR, gls_ret_##FUNCNAME##_t, FUNCNAME);    \
  //
#define GLS_WAIT_SET_RAWRET_PTR(PTR, TYPE, FUNCNAME)            \
  GLS_WAIT_RET(GLSC_GLS_UNDEF, FUNCNAME);                       \
  GLS_SET_RAWRET_PTR(PTR, TYPE, FUNCNAME);                      \
  //
#define GLS_WAIT_SET_RET_PTR(PTR, FUNCNAME)                     \
  GLS_WAIT_RET(GLSC_##FUNCNAME, FUNCNAME);                      \
  GLS_SET_RET_PTR(PTR, FUNCNAME);                               \
  //
#define GLS_RELEASE_RET()                                       \
  if (glsc_global.pool.mallocated) {                            \
    free(glsc_global.pool.mallocated);                          \
    glsc_global.pool.mallocated = NULL;                         \
  }                                                             \
  //
#define GLS_RELEASE_RETURN_RET(TYPE, PTR, FIELD)        \
  do {                                                  \
    TYPE tmp = (TYPE)PTR->FIELD;                        \
    GLS_RELEASE_RET();                                  \
    return tmp;                                         \
  } while(0)


#define WARN_ONCE(FMT, ...) do {         \
    static int shown = 0;                \
    if (!shown) {                        \
      LOGW(FMT, __VA_ARGS__);            \
      shown = 1;                         \
    }                                    \
  } while(0);                            \
  //

#define WARN_STUBBED() WARN_ONCE("%s is stubbed\n", __FUNCTION__)
#define WARN_UNTESTED() WARN_ONCE("%s is untested\n", __FUNCTION__)

#define TRUE 1
#define FALSE 0

extern gls_context_t glsc_global;
extern uint32_t client_egl_error;
extern uint32_t client_gles_error;

void gls_init_library(void);
void gls_cleanup_library(void);

extern gls_context_t glsc_global;
int send_packet(struct xmitr* xmitr);
int gls_cmd_send_data(struct xmitr* xmitr, uint32_t size, const void* data);
int wait_for_data(enum GL_Server_Command, char* str);
void gls_cmd_CREATE_WINDOW(NativeWindowType w, unsigned width, unsigned height);

void* gls_GetEglProcAddress(const char* procname);
void* gls_GetGlesProcAddress(const char* procname);
