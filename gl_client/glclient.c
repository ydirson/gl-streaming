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
#include "transport.h"
#include "fastlog.h"
#include "ltt_wrappers.h"
#include "ltt_api_tp.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

gls_context_t glsc_global;
uint32_t client_egl_error;
uint32_t client_gles_error;

static const int GLS_TIMEOUT_MSEC = 3000;

static int gls_init(void)
{
  if (GLS_VERSION & 1)
    LOGW("this is a development GLS protocol, "
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
    LOGE("GLS_DEBUG variable must be 0 or 1\n");
    return FALSE;
  }

  glsc_global.pack_alignment = 4;
  glsc_global.unpack_alignment = 4;
  glsc_global.pool.out_buf.buf = (char*)malloc(GLS_OUT_BUFFER_SIZE);
  if (glsc_global.pool.out_buf.buf == NULL) {
    LOGE("failed to allocate out_buf: %s\n", strerror(errno));
    return FALSE;
  }
  glsc_global.pool.tmp_buf.buf = (char*)malloc(GLS_TMP_BUFFER_SIZE);
  if (glsc_global.pool.tmp_buf.buf == NULL) {
    LOGE("failed to allocate tmp_buf: %s\n", strerror(errno));
    free(glsc_global.pool.out_buf.buf);
    return FALSE;
  }
  glsc_global.pool.out_buf.size = GLS_OUT_BUFFER_SIZE;
  glsc_global.pool.tmp_buf.size = GLS_TMP_BUFFER_SIZE;

  return TRUE;
}


static int gls_free(void)
{
  free(glsc_global.pool.out_buf.buf);
  free(glsc_global.pool.tmp_buf.buf);

  return TRUE;
}


int send_packet(void)
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

  if (tport_write(glsc_global.rc.cnx, glsc_global.pool.out_buf.buf, c->cmd_size) < 0) {
    switch (c->cmd & GLSC_PROTOCOL_MASK) {
    case GLSC_PROTOCOL_EGL:
      client_egl_error = EGL_BAD_ACCESS; // dubious but eh
      break;
    case GLSC_PROTOCOL_GLES2:
      client_gles_error = GL_INVALID_OPERATION; // dubious but eh
      break;
    }
    tport_close(glsc_global.rc.cnx);
    return FALSE;
  }
  return TRUE;
}

static int handle_packet(enum GL_Server_Command cmd)
{
  int ret = -1;
  void* popptr = (void*)ring_pop_ptr_get(&glsc_global.rc.ring);
  if (popptr == NULL) {
    LOGE("handle_packet: no packet\n");
    return 0;
  }

  gls_command_t* c = (gls_command_t*)popptr;
  switch (c->cmd) {
  case GLSC_SEND_DATA: {
    gls_cmd_send_data_t* data = (gls_cmd_send_data_t*)c;
    if (data->dataptr)
      data = (gls_cmd_send_data_t*)data->dataptr;
    gls_command_t* ret = (gls_command_t*)data->data;
    if (cmd != GLSC_GLS_UNDEF && ret->cmd != cmd) {
      LOGE("received DATA packet has wrong command, 0x%x (%s) != 0x%x (%s)\n",
           ret->cmd, GLSC_tostring(ret->cmd), cmd, GLSC_tostring(cmd));
      break; // ignore packet and try again, but no luck
    }
    if (ringbuf_data_to_bufpool(&glsc_global.pool, &glsc_global.rc.ring, c))
      ret = 0;
    break;
  }
  default:
    LOGE("received non-DATA packet, cmd=0x%x (%s)\n",
         c->cmd, GLSC_tostring(c->cmd));
    break;
  }
  ring_pop_ptr_next(&glsc_global.rc.ring);
  return ret;
}

int wait_for_data(enum GL_Server_Command cmd, char* str)
{
  if (glsc_global.is_debug) LOGD("wait_for_data(%s)\n", str);

  enum {
    POLLFD_RING,
  };
  struct pollfd pollfds[] = {
    [POLLFD_RING] = {
      .fd = glsc_global.rc.ring.pipe_rd,
      .events = POLLIN
    },
  };

  while (1) {
    int ret = poll(pollfds, sizeof(pollfds) / sizeof(pollfds[0]), GLS_TIMEOUT_MSEC);
    if (ret == 0) {
      LOGE("timeout:%s\n", str);
      break;
    }
    if (ret < 0) {
      LOGE("poll failed: %s\n", strerror(errno));
      break;
    }
    assert(!(pollfds[POLLFD_RING].revents & POLLNVAL));

    if (pollfds[POLLFD_RING].revents & POLLERR) {
      LOGE("ring poll error\n");
      break;
    }
    if (pollfds[POLLFD_RING].revents & POLLHUP) {
      LOGD("ring poll hangup\n");
      break;
    }
    if (pollfds[POLLFD_RING].revents & POLLIN) {
      handle_packet(cmd);
      pollfds[POLLFD_RING].revents &= ~POLLIN;
      return TRUE;
    }
    if (pollfds[POLLFD_RING].revents) {
      LOGW("ring poll revents=0x%x\n", pollfds[POLLFD_RING].revents);
      break;
    }
    // should not happen
    LOGE("wait_for_data internal error\n");
    break;
  }
  exit(EXIT_FAILURE);
}


int gls_cmd_send_data(uint32_t size, const void* data)
{
  if (glsc_global.is_debug) LOGD("%s\n", __FUNCTION__);
  gls_cmd_send_data_t* c = (gls_cmd_send_data_t*)glsc_global.pool.out_buf.buf;
  c->cmd = GLSC_SEND_DATA;
  c->cmd_size = sizeof(gls_cmd_send_data_t) + size;
  c->zero = 0;

  struct iovec iov[] = {
    { c, sizeof(gls_cmd_send_data_t) },
    { (void*)data, size }
  };

  if (tport_writev(glsc_global.rc.cnx, iov, sizeof(iov)/sizeof(iov[0])) < 0) {
    client_egl_error = EGL_BAD_ACCESS; // dubious but eh
    tport_close(glsc_global.rc.cnx);
    return FALSE;
  }
  return TRUE;
}


static int gls_cmd_HANDSHAKE(void)
{
  tracepoint(gls_api, call, GLSC_HANDSHAKE);
  GLS_SET_COMMAND_PTR(c, HANDSHAKE);
  if (!send_packet())
    return FALSE;

  GLS_WAIT_SET_RET_PTR(ret, HANDSHAKE);
  if (ret->server_version != GLS_VERSION) {
    LOGE("Incompatible version, server version %i but client version %i.\n",
         ret->server_version, GLS_VERSION);
    GLS_RELEASE_RET();
    tracepoint(gls_api, calldone, GLSC_HANDSHAKE);
    return FALSE;
  }

  GLS_RELEASE_RET();
  tracepoint(gls_api, calldone, GLSC_HANDSHAKE);
  return TRUE;
}

void gls_cmd_CREATE_WINDOW(NativeWindowType w, unsigned width, unsigned height)
{
  GLS_SET_COMMAND_PTR(c, CREATE_WINDOW);
  c->window = w;
  c->width = width;
  c->height = height;
  GLS_SEND_PACKET(CREATE_WINDOW);
}

// client settings
#define RING_SIZE_ORDER 2
#define RING_PACKET_SIZE_ORDER 10

static void recvr_client_start(recvr_context_t* rc, const char* server_addr)
{
  ring_init(&rc->ring, RING_SIZE_ORDER, RING_PACKET_SIZE_ORDER);

  rc->cnx = tport_client_create(server_addr);
  if (!rc->cnx)
    exit(EXIT_FAILURE);

  recvr_run_loop(rc);
}

static int gls_initialized = FALSE;

void gls_init_library(void)
{
  if (gls_initialized)
    return;

  init_lttng_tracepoints();

  if (tport_select(getenv("GLS_TRANSPORT")) < 0) {
    LOGE("cannot select transport\n");
    exit(EXIT_FAILURE);
  }

  recvr_client_start(&glsc_global.rc, getenv("GLS_SERVER_ADDR"));
  if (!gls_init())
    exit(EXIT_FAILURE);
  if (!gls_cmd_HANDSHAKE())
    exit(EXIT_FAILURE);

  gls_initialized = TRUE;
}

void gls_cleanup_library(void)
{
  if (!gls_initialized) return;
  recvr_stop(&glsc_global.rc);
  gls_free();
}
