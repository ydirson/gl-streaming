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

#define _GNU_SOURCE
#include "glclient.h"
#include "xmitr.h"
#include "transport.h"
#include "fastlog.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>

gls_context_t glsc_global;
uint32_t client_egl_error;
uint32_t client_gles_error;

static const int GLS_TIMEOUT_MSEC = 3000;

int send_packet(struct xmitr* xmitr)
{
  char* out_buf = xmitr_getbuf(xmitr);
  gls_command_t* c = (gls_command_t*)out_buf;

  // reset clientside error for protocol
  switch (c->cmd & GLSC_PROTOCOL_MASK) {
  case GLSC_PROTOCOL_EGL:
    client_egl_error = EGL_SUCCESS;
    break;
  case GLSC_PROTOCOL_GLES2:
    client_gles_error = GL_NO_ERROR;
    break;
  }

  if (xmitr_sendbuf(xmitr, c->cmd_size) < 0) {
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

static int send_packet_fd(struct xmitr* xmitr, int fd, uint32_t cmd, size_t size)
{
  // reset clientside error for protocol
  switch (cmd & GLSC_PROTOCOL_MASK) {
  case GLSC_PROTOCOL_EGL:
    client_egl_error = EGL_SUCCESS;
    break;
  case GLSC_PROTOCOL_GLES2:
    client_gles_error = GL_NO_ERROR;
    break;
  }

  // send in 2 operations, so the fd is not attached to the header
  char* out_buf = xmitr_getbuf(xmitr);
  int ret = tport_write(glsc_global.rc.cnx, out_buf, sizeof(gls_command_t));
  if (ret > 0)
    ret = tport_write_fd(glsc_global.rc.cnx, out_buf + sizeof(gls_command_t),
                         size - sizeof(gls_command_t), fd);
  if (ret < 0) {
    switch (cmd & GLSC_PROTOCOL_MASK) {
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
      .fd = notifier_fd(&glsc_global.rc.ring.notifier),
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
      if (notifier_has_terminated(&glsc_global.rc.ring.notifier)) {
        LOGD("RING notifier terminated\n");
        break;
      }
      uint64_t ret = notifier_drain(&glsc_global.rc.ring.notifier);
      if (ret == 0) {
        LOGE("RING notifier drain error: %s\n", strerror(errno));
        break;
      }
      for (uint64_t i = 0; i < ret; i++)
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


int gls_cmd_send_data(struct xmitr* xmitr, uint32_t size, const void* data)
{
  if (glsc_global.is_debug) LOGD("%s\n", __FUNCTION__);
  if (xmitr_senddata(xmitr, data, size) < 0) {
    client_egl_error = EGL_BAD_ACCESS; // dubious but eh
    return FALSE;
  }
  return TRUE;
}


static int gls_cmd_HANDSHAKE(void)
{
  if (glsc_global.is_debug) LOGD("%s\n", __FUNCTION__);
  GLS_SET_COMMAND_PTR(c, HANDSHAKE);
  if (!send_packet(glsc_global.cmd_xmitr))
    return FALSE;

  GLS_WAIT_SET_RET_PTR(ret, HANDSHAKE);
  if (ret->server_version != GLS_VERSION) {
    LOGE("Incompatible version, server version %i but client version %i.\n",
         ret->server_version, GLS_VERSION);
    GLS_RELEASE_RET();
    return FALSE;
  }

  GLS_RELEASE_RET();
  return TRUE;
}

void gls_cmd_CREATE_WINDOW(NativeWindowType w, unsigned width, unsigned height)
{
  if (glsc_global.is_debug) LOGD("%s\n", __FUNCTION__);
  GLS_SET_COMMAND_PTR(c, CREATE_WINDOW);
  c->window = w;
  c->width = width;
  c->height = height;
  GLS_SEND_PACKET(CREATE_WINDOW);
}

static int gls_cmd_SHARE_SHM(int fd, uint32_t size)
{
  if (glsc_global.is_debug) LOGD("%s\n", __FUNCTION__);
  GLS_SET_COMMAND_PTR(c, SHARE_SHM);
  c->size = size;
  c->fd = -1; // mostly for safety of the hack
  if (!send_packet_fd(glsc_global.cmd_xmitr, fd, GLSC_SHARE_SHM, c->cmd_size))
    return FALSE;

  GLS_WAIT_SET_RET_PTR(ret, SHARE_SHM);
  GLS_RELEASE_RETURN_RET(int, ret, success);
}

static int shmcreate_malloc(ring_t* ring, void* data)
{
  (void)data; assert(!data); ring->allocator_data = NULL; // unused
  // buffer creation and mapping
  size_t size = ring->ring_size * ring->ring_packet_size;
  int shm_fd = memfd_create("gls-ring", MFD_CLOEXEC);
  if (shm_fd < 0) {
    LOGE("shm ring allocation failure: %s\n", strerror(errno));
    return -1;
  }
  if (ftruncate(shm_fd, size) < 0) {
    LOGE("shm ring sizing failure: %s\n", strerror(errno));
    close(shm_fd);
    return -1;
  }
  ring->buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (!ring->buffer) {
    LOGE("shm ring mapping failure: %s\n", strerror(errno));
    close(shm_fd);
    return -1;
  }

  // share, release fd on our side
  if (!gls_cmd_SHARE_SHM(shm_fd, size)) {
    LOGE("shm ring sharing failed\n");
    munmap(ring->buffer, size);
    close(shm_fd);
    return -1;
  }
  close(shm_fd);

  return 0;
}
static void shmcreate_free(ring_t* ring)
{
  size_t size = ring->ring_size * ring->ring_packet_size;
  munmap(ring->buffer, size);
  free(ring->allocator_data);
}

static ring_allocator_t shmcreate_allocator = {
  .alloc = shmcreate_malloc,
  .free = shmcreate_free,
};

void gls_init_library(void)
{
  static int init = FALSE;
  if (init)
    return;

  if (GLS_VERSION & 1)
    LOGW("this is a development GLS protocol, "
         "make sure client and server match\n");

  if (tport_select(getenv("GLS_TRANSPORT")) < 0) {
    LOGE("cannot select transport\n");
    exit(EXIT_FAILURE);
  }

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
    exit(EXIT_FAILURE);
  }

  // get ready to receive replies
  int ret;
  if (tport_has_offloading())
    ret = ring_init(&glsc_global.rc.ring, NULL, NULL,
                    CMD_RING_SIZE_ORDER, CMD_RING_PACKET_SIZE_ORDER);
  else
    ret = ring_init(&glsc_global.rc.ring, NULL, NULL,
                    SRV2CLT_API_RING_SIZE_ORDER, SRV2CLT_API_RING_PACKET_SIZE_ORDER);
  if (ret < 0)
    exit(EXIT_FAILURE);

  // open cnx - FIXME logically part of xmitr_stream_init
  glsc_global.rc.cnx = tport_client_create(getenv("GLS_SERVER_ADDR"));
  if (!glsc_global.rc.cnx)
    exit(EXIT_FAILURE);

  // get ready to transmit
  glsc_global.pack_alignment = 4;
  glsc_global.unpack_alignment = 4;
  glsc_global.cmd_xmitr = xmitr_stream_init(glsc_global.rc.cnx);
  if (!glsc_global.cmd_xmitr) {
    LOGE("failed to init cmd xmitr\n");
    exit(EXIT_FAILURE);
  }

  // get pool ready for reception of replies
  // FIXME would need to move in a better place
  glsc_global.pool.tmp_buf.buf = (char*)malloc(GLS_TMP_BUFFER_SIZE);
  if (glsc_global.pool.tmp_buf.buf == NULL) {
    LOGE("failed to allocate tmp_buf: %s\n", strerror(errno));
    xmitr_free(glsc_global.api_xmitr);
    exit(EXIT_FAILURE);
  }
  glsc_global.pool.tmp_buf.size = GLS_TMP_BUFFER_SIZE;

  // start command transmission
  recvr_run_loop(&glsc_global.rc);
  if (!gls_cmd_HANDSHAKE())
    exit(EXIT_FAILURE);

  if (tport_has_offloading()) {
    // negociate an API xmitr
    glsc_global.api_xmitr = xmitr_shm_init(&shmcreate_allocator, NULL);
    if (!glsc_global.api_xmitr) {
      LOGE("failed to init API xmitr\n");
      exit(EXIT_FAILURE);
    }
  } else
    // share the command xmitr
    glsc_global.api_xmitr = glsc_global.cmd_xmitr;

  init = TRUE;
}

void gls_cleanup_library(void)
{
  recvr_stop(&glsc_global.rc);

  if (glsc_global.api_xmitr != glsc_global.cmd_xmitr)
    xmitr_free(glsc_global.api_xmitr);
  xmitr_free(glsc_global.cmd_xmitr);
  free(glsc_global.pool.tmp_buf.buf);
}
