#include "transport.h"
#include "fastlog.h"

#include <libvchan.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// FIXME
#define CLTDOMAIN 6

struct gls_connection
{
  libvchan_t* vchan;
};

static struct gls_connection* qubesvchan_tport_connection_create(void)
{
  // FIXME parse_address
  // domid, port
  unsigned port = 18145;

  libvchan_t* vchan;
  // client is server for the shared pages
#ifdef GLS_SERVER
  int clt_domid = CLTDOMAIN;
  vchan = libvchan_client_init(clt_domid, port);
#else
  int srv_domid = 0; // FIXME
  vchan = libvchan_server_init(srv_domid, port,
                               1 << 14, // FIXME: SIZE_ORDER's in glclient.c -- was 12
                               1 << 20); // FIXME: SIZE_ORDER's in main.c - we would like 27
#endif
  if (!vchan) {
    LOGE("qubesvchan init failed: %s\n", strerror(errno)); // FIXME is errno meaningful?
    return NULL;
  }

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("malloc failed: %s\n", strerror(errno));
    return NULL;
  }
  cnx->vchan = vchan;

#ifndef GLS_SERVER
  // wait for connection from server
  LOGI("waiting for vchan establishment...\n");
  while (libvchan_is_open(vchan) == 2)
    libvchan_wait(vchan);
  assert (libvchan_is_open(vchan) == 1);
#endif
  LOGI("vchan connected\n");

  return cnx;
}

static struct gls_connection* qubesvchan_tport_client_create(const char* server_addr)
{
  LOGI("initializing qubesvchan transport\n");
  (void)server_addr;
  return tport_connection_create();
}

static int qubesvchan_tport_connection_fd(struct gls_connection* cnx)
{
  return libvchan_fd_for_select(cnx->vchan);
}

static ssize_t _qubesvchan_tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = libvchan_write(cnx->vchan, buffer, size);
  if (ret < 0)
    LOGE("libvchan_write failed\n");
  return ret;
}

static ssize_t qubesvchan_tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = _qubesvchan_tport_write(cnx, buffer, size);
  if ((size_t)ret < size)
    LOGW("%s: short write %zd < %zu\n", __FUNCTION__, ret, size);
  return ret;
}

static ssize_t qubesvchan_tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt)
{
  int i;
  size_t written = 0;
  for (i = 0; i < iovcnt; i++) {
    ssize_t ret = _qubesvchan_tport_write(cnx, iov[i].iov_base, iov[i].iov_len);
    if (ret < 0) {
      LOGE("_qubesvchan_tport_write failed for writev\n");
      return ret;
    }
    written += ret;
    if ((size_t)ret < iov[i].iov_len)
      break;
  }
  return written;
}

static ssize_t qubesvchan_tport_read(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = libvchan_read(cnx->vchan, buffer, size);
  if (ret < 0) {
     // FIXME should check this right after select()
    if (libvchan_is_open(cnx->vchan) == VCHAN_DISCONNECTED) {
      LOGI("EOF on libvchan\n");
      return 0;
    }
    LOGE("libvchan_read failed: %s\n", strerror(errno));
    abort();
  }
  return ret;
}

static void qubesvchan_tport_close(struct gls_connection* cnx)
{
  libvchan_close(cnx->vchan);
}

struct gls_transport gls_tport_qubesvchan = {
  .name = "qubesvchan",
  .connection_create = qubesvchan_tport_connection_create,
  .client_create = qubesvchan_tport_client_create,
  .connection_fd = qubesvchan_tport_connection_fd,
  .write = qubesvchan_tport_write,
  .writev = qubesvchan_tport_writev,
  .read = qubesvchan_tport_read,
  .close = qubesvchan_tport_close,
};
