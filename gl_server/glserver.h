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

#define TRUE 1
#define FALSE 0

#define GLSE_TMP_BUFFER_SIZE 2097152
#define GLSE_OUT_BUFFER_SIZE 4096 // 2048

#define GLSE_SET_COMMAND_PTR(PTR, FUNCNAME) \
  gls_##FUNCNAME##_t *PTR = (gls_##FUNCNAME##_t *)buf;

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
