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
#include "fastlog.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// #define GL_DEBUG

#ifdef GL_DEBUG
#include <stdio.h>
#endif

glse_context_t glsec_global;


void pop_batch_command(size_t size)
{
  glsec_global.tmp_buf.ptr = next_ptr(glsec_global.tmp_buf.ptr, size, GLS_ALIGNMENT_BITS);
}


static int send_packet(size_t size)
{
  if (send(glsec_global.rc->sock_fd, glsec_global.out_buf.buf, size, 0) < 0) {
    fprintf(stderr, "GLS ERROR: send_packet failure: %s\n", strerror(errno));
    return FALSE;
  }
  return TRUE;
}


int glse_cmd_send_data(uint32_t size, void *data)
{
#ifdef GL_DEBUG
  fprintf(stderr, "GLS: glse_cmd_send_data sending data back\n");
#endif
  gls_cmd_send_data_t *c = (gls_cmd_send_data_t *)glsec_global.out_buf.buf;
  c->cmd = GLSC_SEND_DATA;

  int success = TRUE;
  uint32_t glssize = GLS_DATA_SIZE * 4;
  char *data1 = (char *)data;
  
  uint32_t offset1;
  for (offset1 = 0; offset1 < size; offset1 += glssize)
  {
    unsigned int size1 = size - offset1;
    c->isLast = (size1 > glssize) ? FALSE : TRUE;
    size1 = (size1 > glssize) ? glssize : size1;
    memcpy(c->data.data_char, data1, size1);
    c->cmd_size = (size_t)(&c->data.data_char[size1] - (char *)c);
    c->offset = offset1;
    c->size = size1;
    if (send_packet(c->cmd_size) == FALSE)
    {
      success = FALSE;
    }
    data1 += glssize;
  }
  return success;
}


void glse_cmd_HANDSHAKE(gls_command_t* buf)
{
  gls_HANDSHAKE_t *c = (gls_HANDSHAKE_t *)buf;
  graphics_context_t *gc = glsec_global.gc;

  gls_ret_HANDSHAKE_t *ret = (gls_ret_HANDSHAKE_t *)glsec_global.tmp_buf.buf;
  ret->cmd = c->cmd;
  ret->cmd_size = sizeof(gls_ret_HANDSHAKE_t);
  ret->screen_width = gc->screen_width = glsurfaceview_width;
  ret->screen_height = gc->screen_height = glsurfaceview_height;
  ret->server_version = GLS_VERSION;
  size_t size = sizeof(gls_ret_HANDSHAKE_t);
  glse_cmd_send_data(size, glsec_global.tmp_buf.buf);
}


void glserver_handle_packets(recvr_context_t* rc)
{
  int quit = FALSE;
  static graphics_context_t gc;
  memset(&glsec_global, 0, sizeof(glsec_global));
  memset(&gc, 0, sizeof(gc));
  init_egl(&gc);

  glsec_global.rc = rc;
  glsec_global.gc = &gc;
  glsec_global.tmp_buf.buf = (char *)malloc(GLSE_TMP_BUFFER_SIZE);
  glsec_global.tmp_buf.size = GLSE_TMP_BUFFER_SIZE;
  glsec_global.out_buf.buf = (char *)malloc(GLSE_OUT_BUFFER_SIZE);
  glsec_global.out_buf.size = GLSE_OUT_BUFFER_SIZE;
  
  while (!quit) {
    void *popptr = (void *)fifo_pop_ptr_get(&rc->fifo);
    if (popptr == NULL) {
      usleep(rc->sleep_usec);
      continue;
    }

    gls_command_t *c = (gls_command_t *)popptr;
#ifdef GL_DEBUG
    fprintf(stderr, "GLS MainLoop: Attempting to execute command 0x%x (%s)\n",
            c->cmd, GLSC_tostring(c->cmd));
#endif

    switch (c->cmd) {
    case GLSC_SEND_DATA:
      fifobuf_data_to_bufpool(&glsec_global.tmp_buf, &glsec_global.rc->fifo, c);
      break;
    case GLSC_HANDSHAKE:
#ifdef GL_DEBUG
      fprintf(stderr, "GLS Exec: Handshake...\n");
#endif
      glse_cmd_HANDSHAKE(c);
      break;
          
    default: {
      int result = FALSE;
      if (!result) result = gles_flushCommand(c); // used-to-be-batched
      if (!result) result = gles_executeCommand(c);
      if (!result) result = egl_executeCommand(c);

      if (!result)
        LOGE("GLS ERROR: Unhandled command 0x%x (%s)\n", c->cmd, GLSC_tostring(c->cmd));
    }
    }
    fifo_pop_ptr_next(&rc->fifo);
  }

  release_egl(&gc);

  free(glsec_global.tmp_buf.buf);
  free(glsec_global.out_buf.buf);
  pthread_exit(NULL);
}


