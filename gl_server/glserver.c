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
#include "glserver.h"
#include "xmitr.h"
#include "transport.h"
#include "fastlog.h"

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// #define GL_DEBUG

glse_context_t glsec_global;


int glse_cmd_send_data(uint32_t size, void* data)
{
#ifdef GL_DEBUG
  LOGD("glse_cmd_send_data sending data back\n");
#endif
  if (xmitr_senddata(glsec_global.xmitr, data, size) < 0)
    return FALSE;

  return TRUE;
}


static void glse_cmd_HANDSHAKE(gls_command_t* buf)
{
  //GLSE_SET_COMMAND_PTR(c, HANDSHAKE);
  (void)buf;

  GLSE_SET_RET_PTR(ret, HANDSHAKE);
  ret->server_version = GLS_VERSION;
  GLSE_SEND_RET(ret, HANDSHAKE);
}


static void glse_cmd_CREATE_WINDOW(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, CREATE_WINDOW);
  graphics_context_t* gc = &glsec_global.gc;
  if (!c->window)
    // invalid at X11 level, here mostly a safeguard for previous single-window code
    LOGW("CREATE_WINDOW referencing NULL Window");
  Window w = gls_create_x11_window(gc, "OpenGL ES 2.x streaming", 0, 0, c->width, c->height);
  list_insert(gc->x.remote2local, c->window, (void*)w);
  // FIXME we have no DESTROY_WINDOW event to remove the mapping
}

static int shmattach_malloc(ring_t* ring, void* data)
{
  gls_SHARE_SHM_t* c = (gls_SHARE_SHM_t*)data;
  //size_t size = ring->ring_size * ring->ring_packet_size;
  ring->buffer = mmap(NULL, c->size, PROT_READ | PROT_WRITE, MAP_SHARED, c->fd, 0);
  if (!ring->buffer) {
    LOGE("shm ring mapping failure: %s\n", strerror(errno));
    close(c->fd); // FIXME: better make sure it is close by called
    return -1;
  }
  close(c->fd); // FIXME: same

  return 0;
}
static void shmattach_free(ring_t* ring)
{
  size_t size = ring->ring_size * ring->ring_packet_size;
  munmap(ring->buffer, size);
  free(ring->allocator_data);
}

static ring_allocator_t shmattach_allocator = {
  .alloc = shmattach_malloc,
  .free = shmattach_free,
};

static void glse_cmd_SHARE_SHM(gls_command_t* buf, recvr_context_t* rc)
{
  int success = 0;
  GLSE_SET_COMMAND_PTR(c, SHARE_SHM);
  LOGD("SHARE_SHM received, fd=%d, size=%u\n", c->fd, c->size);
  if (c->fd < 0) {
    LOGE("SHARE_SHM with no valid fd\n");
    goto reply;
  }
  // FIXME buffer sizing should come from msg
  // FIXME api_ring must come from tport but this looks fishy -- likely xmitr instead
  int res = ring_init(tport_api_ring(rc->cnx), &shmattach_allocator, (void*)c,
                      CLT2SRV_API_RING_SIZE_ORDER, CLT2SRV_API_RING_PACKET_SIZE_ORDER);
  if (res < 0)
    goto reply;
  LOGD("SHARE_SHM created api_ring\n");
  success = 1;

 reply: ;
  close(c->fd); // must be closed in both error/success cases
  GLSE_SET_RET_PTR(ret, SHARE_SHM);
  ret->success = success;
  GLSE_SEND_RET(ret, SHARE_SHM);
}

static void glse_handle_cmd_ring_packet(recvr_context_t* rc)
{
  void* popptr = (void*)ring_pop_ptr_get(&rc->ring);
  if (popptr == NULL) { // should not happen, poll() rocks
    LOGW("glse_handle_cmd_ring_packet called with empty ring\n");
    return;
  }

  gls_command_t* c = (gls_command_t*)popptr;
#ifdef GL_DEBUG
  LOGD("mainloop: Attempting to execute command 0x%x (%s)\n",
          c->cmd, GLSC_tostring(c->cmd));
#endif

  switch (c->cmd) {
  case GLSC_SEND_DATA:
    ringbuf_data_to_bufpool(&glsec_global.pool, &rc->ring, c);
    break;
  case GLSC_HANDSHAKE:
#ifdef GL_DEBUG
    LOGD("executing: Handshake...\n");
#endif
    glse_cmd_HANDSHAKE(c);
    break;
  case GLSC_CREATE_WINDOW:
#ifdef GL_DEBUG
    LOGD("executing: Create window...\n");
#endif
    glse_cmd_CREATE_WINDOW(c);
    break;
  case GLSC_SHARE_SHM:
    glse_cmd_SHARE_SHM(c, rc);
    break;

  default: {
    int result = FALSE;
    if (!result) result = gles_executeCommand(c);
    if (!result) result = egl_executeCommand(c);

    if (!result)
      LOGE("Unhandled command 0x%x (%s)\n", c->cmd, GLSC_tostring(c->cmd));
  }
  }
  ring_pop_ptr_next(&rc->ring);
}

// FIXME essentially dup of glse_handle_cmd_ring_packet
static void glse_handle_api_ring_packet(ring_t* ring)
{
  void* popptr = (void*)ring_pop_ptr_get(ring);
  if (popptr == NULL) { // should not happen, poll() rocks
    LOGW("glse_handle_api_ring_packet called with empty ring\n");
    return;
  }

  gls_command_t* c = (gls_command_t*)popptr;
#ifdef GL_DEBUG
  LOGD("mainloop: Attempting to execute command 0x%x (%s)\n",
          c->cmd, GLSC_tostring(c->cmd));
#endif

  switch (c->cmd) {
  case GLSC_SEND_DATA:
    ringbuf_data_to_bufpool(&glsec_global.pool, ring, c);
    break;
  case GLSC_CREATE_WINDOW:
#ifdef GL_DEBUG
    LOGD("executing: Create window...\n");
#endif
    glse_cmd_CREATE_WINDOW(c);
    break;

  default: {
    int result = FALSE;
    if (!result) result = gles_executeCommand(c);
    if (!result) result = egl_executeCommand(c);

    if (!result)
      LOGE("Unhandled command 0x%x (%s)\n", c->cmd, GLSC_tostring(c->cmd));
  }
  }
  ring_pop_ptr_next(ring);
}

void glserver_handle_packets(recvr_context_t* rc)
{
  init_egl(&glsec_global.gc);

  glsec_global.pool.tmp_buf.buf = (char*)malloc(GLSE_TMP_BUFFER_SIZE);
  glsec_global.pool.tmp_buf.size = GLSE_TMP_BUFFER_SIZE;
  glsec_global.xmitr = xmitr_stream_init(rc->cnx);

  ring_t* api_ring = tport_api_ring(rc->cnx);

  enum {
    POLLFD_CMD_RING,
    POLLFD_API_RING,
  };
  struct pollfd pollfds[] = {
    [POLLFD_CMD_RING] = {
      .fd = notifier_fd(&rc->ring.notifier),
      .events = POLLIN
    },
    [POLLFD_API_RING] = {
      .fd = tport_has_offloading() ? notifier_fd(&api_ring->notifier) : -1,
      .events = POLLIN
    },
  };

  while (1) {
    int ret = poll(pollfds, sizeof(pollfds) / sizeof(pollfds[0]), -1);
    if (ret < 0) {
      LOGE("poll failed: %s\n", strerror(errno));
      break;
    }
    assert(!(pollfds[POLLFD_CMD_RING].revents & POLLNVAL));
    assert(!(pollfds[POLLFD_API_RING].revents & POLLNVAL));

    // POLLFD_CMD_RING
    if (pollfds[POLLFD_CMD_RING].revents & POLLERR) {
      LOGE("CMD ring poll error\n");
      break;
    }
    if (pollfds[POLLFD_CMD_RING].revents & POLLHUP) {
      //LOGD("CMD ring poll hangup\n");
      break;
    }
    if (pollfds[POLLFD_CMD_RING].revents & POLLIN) {
      if (notifier_has_terminated(&rc->ring.notifier)) {
        LOGD("CMD ring notifier terminated\n");
        break;
      }
      uint64_t ret = notifier_drain(&rc->ring.notifier);
      if (ret == 0) {
        LOGE("CMD ring notifier drain error: %s\n", strerror(errno));
        break;
      }
      for (uint64_t i = 0; i < ret; i++)
        glse_handle_cmd_ring_packet(rc);
      pollfds[POLLFD_CMD_RING].revents &= ~POLLIN;
    }
    if (pollfds[POLLFD_CMD_RING].revents)
      LOGW("CMD ring poll revents=0x%x\n", pollfds[POLLFD_CMD_RING].revents);

    // POLLFD_API_RING
    if (pollfds[POLLFD_API_RING].revents & POLLERR) {
      LOGE("API ring poll error\n");
      break;
    }
    if (pollfds[POLLFD_API_RING].revents & POLLHUP) {
      //LOGD("API ring poll hangup\n");
      break;
    }
    if (pollfds[POLLFD_API_RING].revents & POLLIN) {
      if (notifier_has_terminated(&api_ring->notifier)) {
        LOGD("API ring notifier terminated\n");
        break;
      }
      uint64_t ret = notifier_drain(&api_ring->notifier);
      if (ret == 0) {
        LOGE("API ring notifier drain error: %s\n", strerror(errno));
        break;
      }
      for (uint64_t i = 0; i < ret; i++)
        glse_handle_api_ring_packet(api_ring);
      pollfds[POLLFD_API_RING].revents &= ~POLLIN;
    }
    if (pollfds[POLLFD_API_RING].revents)
      LOGW("API ring poll revents=0x%x\n", pollfds[POLLFD_API_RING].revents);
  }

  release_egl(&glsec_global.gc);

  free(glsec_global.pool.tmp_buf.buf);
  xmitr_free(glsec_global.xmitr);
}


int glse_extension_supported(const char** allowed_list, const char* name, size_t namelen)
{
  static int dont_filter = -1;
  if (dont_filter == -1) {
    const char* envstr = getenv("GLS_DONT_FILTER");
    if (!envstr || atoi(envstr) == 0) {
      dont_filter = 0;
    } else {
      dont_filter = 1;
      LOGI("not filtering unsupported extensions, expect breakage\n");
    }
  }
  if (dont_filter)
    return 1;
  while (*allowed_list) {
    // match exactly, not just as a prefix
    if (strncmp(*allowed_list, name, namelen) == 0 && (*allowed_list)[namelen] == '\0')
      return 1;
    allowed_list++;
  }
  return 0;
}
