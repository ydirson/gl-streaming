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

#include "glclient.h"

#include "EGL/egl.h"
#include "GLES2/gl2.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

gls_context_t glsc_global;
uint32_t client_egl_error;
uint32_t client_gles_error;

static float get_diff_time(struct timeval start, struct timeval end)
{
  float dt = (float)(end.tv_sec - start.tv_sec) + (float)(end.tv_usec - start.tv_usec) * 0.000001f;
  return dt;
}


static int gls_init()
{
  if (GLS_VERSION & 1)
    fprintf(stderr, "WARNING: this is a development GLS protocol, "
            "make sure client and server match\n");

  const char* env_isDebugStr = getenv("GLS_DEBUG");
  int env_isDebug;
  if (env_isDebugStr == NULL) {
    env_isDebug = FALSE;
  } else {
    env_isDebug = atoi(env_isDebugStr);
  }
  if (env_isDebug == 0 || env_isDebug == 1) {
    glsc_global.is_debug = env_isDebug;
  } else {
    fprintf(stderr, "gls error: GLS_DEBUG variable must be 0 or 1\n");
    exit(EXIT_FAILURE);
    return FALSE;
  }

  glsc_global.pack_alignment = 4;
  glsc_global.unpack_alignment = 4;
  glsc_global.pool.out_buf.buf = (char*)malloc(GLS_OUT_BUFFER_SIZE);
  if (glsc_global.pool.out_buf.buf == NULL) {
    return FALSE;
  }
  glsc_global.pool.tmp_buf.buf = (char*)malloc(GLS_TMP_BUFFER_SIZE);
  if (glsc_global.pool.tmp_buf.buf == NULL) {
    free(glsc_global.pool.out_buf.buf);
    return FALSE;
  }
  glsc_global.pool.out_buf.size = GLS_OUT_BUFFER_SIZE;
  glsc_global.pool.tmp_buf.size = GLS_TMP_BUFFER_SIZE;

  return TRUE;
}


static int gls_free()
{
  free(glsc_global.pool.out_buf.buf);
  free(glsc_global.pool.tmp_buf.buf);

  return TRUE;
}


int send_packet()
{
  gls_command_t* c = (gls_command_t*)glsc_global.pool.out_buf.buf;

  // reset clientside error for protocol
  switch (c->cmd & GLSC_PROTOCOL_MASK) {
  case GLSC_PROTOCOL_EGL:
    client_egl_error = EGL_SUCCESS;
    break;
  case GLSC_PROTOCOL_GLES2:
    client_gles_error = GL_NO_ERROR;
    break;
  }

  if (send(glsc_global.rc.sock_fd, glsc_global.pool.out_buf.buf, c->cmd_size, 0) < 0) {
    fprintf(stderr, "GLS ERROR: send_packet(%u) failure: %s\n", c->cmd_size, strerror(errno));
    switch (c->cmd & GLSC_PROTOCOL_MASK) {
    case GLSC_PROTOCOL_EGL:
      client_egl_error = EGL_BAD_ACCESS; // dubious but eh
      break;
    case GLSC_PROTOCOL_GLES2:
      client_gles_error = GL_INVALID_OPERATION; // dubious but eh
      break;
    }
    return FALSE;
  }
  return TRUE;
}


int wait_for_data(enum GL_Server_Command cmd, char* str)
{
  if (glsc_global.is_debug) fprintf(stderr, "wait_for_data(%s)\n", str);
  struct timeval start_time, end_time;
  gettimeofday(&start_time, NULL);
  int quit = 0;
  while (!quit) {
    void* popptr = (void*)fifo_pop_ptr_get(&glsc_global.rc.fifo);
    if (popptr == NULL) {
      gettimeofday(&end_time, NULL);
      float diff_time = get_diff_time(start_time, end_time);
      if (diff_time > GLS_TIMEOUT_SEC) {
        fprintf(stderr, "\nGLS ERROR: timeout:%s\n", str);
        exit(EXIT_FAILURE);
        return FALSE;
      }
      usleep(glsc_global.rc.sleep_usec);
      continue;
    }

    gls_command_t* c = (gls_command_t*)popptr;
    switch (c->cmd) {
    case GLSC_SEND_DATA: {
      gls_cmd_send_data_t* data = (gls_cmd_send_data_t*)c;
      if (data->dataptr)
        data = (gls_cmd_send_data_t*)data->dataptr;
      gls_command_t* ret = (gls_command_t*)data->data;
      if (cmd != GLSC_GLS_UNDEF && ret->cmd != cmd) {
        fprintf(stderr,
                "GLS ERROR: received DATA packet has wrong command, 0x%x (%s) != 0x%x (%s)\n",
                ret->cmd, GLSC_tostring(ret->cmd), cmd, GLSC_tostring(cmd));
        break; // ignore packet and try again, but no luck
      }
      if (fifobuf_data_to_bufpool(&glsc_global.pool, &glsc_global.rc.fifo, c))
        quit = TRUE;
      break;
      }
    default:
      fprintf(stderr, "GLS ERROR: received non-DATA packet, cmd=0x%x (%s)\n",
              c->cmd, GLSC_tostring(c->cmd));
      break;
    }
    fifo_pop_ptr_next(&glsc_global.rc.fifo);
  }
  return TRUE;
}


int gls_cmd_send_data(uint32_t size, const void* data)
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  gls_cmd_send_data_t* c = (gls_cmd_send_data_t*)glsc_global.pool.out_buf.buf;
  c->cmd = GLSC_SEND_DATA;
  c->cmd_size = sizeof(gls_cmd_send_data_t) + size;
  c->zero = 0;

  struct iovec iov[2] = {
    { c, sizeof(gls_cmd_send_data_t) },
    { (void*)data, size }
  };
  struct msghdr msg = { .msg_iov = iov, .msg_iovlen = 2 };

  if (sendmsg(glsc_global.rc.sock_fd, &msg, 0) < 0) {
    fprintf(stderr, "GLS ERROR: send_data sendmsg(%u) failure: %s\n", c->cmd_size, strerror(errno));
    client_egl_error = EGL_BAD_ACCESS; // dubious but eh
    return FALSE;
  }
  return TRUE;
}


static int gls_cmd_HANDSHAKE()
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  GLS_SET_COMMAND_PTR(c, HANDSHAKE);
  if (!send_packet())
    return FALSE;

  GLS_WAIT_SET_RET_PTR(ret, HANDSHAKE);
  if (ret->server_version != GLS_VERSION) {
    fprintf(stderr, "GLS ERROR: Incompatible version, server version %i but client version %i.\n", ret->server_version, GLS_VERSION);
    GLS_RELEASE_RET();
    return FALSE;
  }

  GLS_RELEASE_RET();
  return TRUE;
}

void gls_cmd_CREATE_WINDOW(unsigned width, unsigned height)
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  GLS_SET_COMMAND_PTR(c, CREATE_WINDOW);
  c->width = width;
  c->height = height;
  GLS_SEND_PACKET(CREATE_WINDOW);
}

void gls_init_library()
{
  static int init = FALSE;
  if (init)
    return;
  char his_ip[30]; // GLS_STRING_SIZE_PLUS
  uint16_t his_port = 18145;

  const char* env_serverAddr = getenv("GLS_SERVER_ADDR");
  if (env_serverAddr == NULL) {
    strncpy(his_ip, "127.0.0.1", 10);
  } else {
    size_t addrlen = strcspn(env_serverAddr, ":");
    assert(addrlen < sizeof(his_ip));
    strncpy(his_ip, env_serverAddr, addrlen);
    his_ip[addrlen] = '\0';

    if (env_serverAddr[addrlen] == ':' && env_serverAddr[addrlen + 1] != '\0')
      his_port = atoi(env_serverAddr + addrlen + 1);
  }
  fprintf(stderr, "GLS INFO: connecting to %s:%u\n", his_ip, his_port);

  recvr_client_start(&glsc_global.rc, his_ip, his_port);
  gls_init();
  if (!gls_cmd_HANDSHAKE())
    exit(EXIT_FAILURE);

  init = TRUE;
}

void gls_cleanup_library()
{
  recvr_stop(&glsc_global.rc);
  gls_free();
}
