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
#include "transport.h"
#include "fastlog.h"

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>

// #define GL_DEBUG

glse_context_t glsec_global;


int glse_cmd_send_data(uint32_t size, void* data)
{
#ifdef GL_DEBUG
  LOGD("glse_cmd_send_data sending data back\n");
#endif
  gls_cmd_send_data_t* c = (gls_cmd_send_data_t*)glsec_global.pool.out_buf.buf;
  c->cmd = GLSC_SEND_DATA;
  c->cmd_size = sizeof(gls_cmd_send_data_t) + size;
  c->zero = 0;

  struct iovec iov[] = {
    { c, sizeof(gls_cmd_send_data_t) },
    { (void*)data, size }
  };

  if (tport_writev(glsec_global.rc.cnx, iov, sizeof(iov)/sizeof(iov[0])) < 0) {
    tport_close(glsec_global.rc.cnx);
    return FALSE;
  }

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

static void glse_handle_ring_packet(recvr_context_t* rc)
{
  void* popptr = (void*)ring_pop_ptr_get(&rc->ring);
  if (popptr == NULL) { // should not happen, poll() rocks
    LOGW("glse_handle_ring_packet called with empty ring\n");
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

void glserver_handle_packets(recvr_context_t* rc)
{
  init_egl(&glsec_global.gc);

  glsec_global.pool.tmp_buf.buf = (char*)malloc(GLSE_TMP_BUFFER_SIZE);
  glsec_global.pool.tmp_buf.size = GLSE_TMP_BUFFER_SIZE;
  glsec_global.pool.out_buf.buf = (char*)malloc(GLSE_OUT_BUFFER_SIZE);
  glsec_global.pool.out_buf.size = GLSE_OUT_BUFFER_SIZE;

  enum {
    POLLFD_RING,
  };
  struct pollfd pollfds[] = {
    [POLLFD_RING] = {
      .fd = rc->ring.pipe_rd,
      .events = POLLIN
    },
  };

  while (1) {
    int ret = poll(pollfds, sizeof(pollfds) / sizeof(pollfds[0]), -1);
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
      //LOGD("ring poll hangup\n");
      break;
    }
    if (pollfds[POLLFD_RING].revents & POLLIN) {
      glse_handle_ring_packet(rc);
      pollfds[POLLFD_RING].revents &= ~POLLIN;
    }
    if (pollfds[POLLFD_RING].revents)
      LOGW("ring poll revents=0x%x\n", pollfds[POLLFD_RING].revents);
  }

  release_egl(&glsec_global.gc);

  free(glsec_global.pool.tmp_buf.buf);
  free(glsec_global.pool.out_buf.buf);
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
