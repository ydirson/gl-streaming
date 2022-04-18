#include "transport.h"
#include "fastlog.h"

#include <libxenvchan.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct gls_connection
{
  struct libxenvchan* vchan;
};

static struct gls_connection* xenvchan_tport_connection_create(void)
{
  // FIXME parse_address
  // domid, xs_path?

  struct libxenvchan* vchan;
  // client is server for the shared pages
#ifdef GLS_SERVER
  int clt_domid = 6; // FIXME
  const char xs_path[] = "/local/domain/6/data/vchan/gls"; // FIXME
  vchan = libxenvchan_client_init(NULL, clt_domid, xs_path);
#else
  int srv_domid = 0; // FIXME
  const char xs_path[] = "data/vchan/gls"; // FIXME only supports a single client
  vchan = libxenvchan_server_init(NULL, srv_domid, xs_path,
                                  1 << 12, // FIXME: SIZE_ORDER's in glclient.c
                                  1 << 20); // FIXME: SIZE_ORDER's in main.c - we would like 27
#endif
  if (!vchan) {
    LOGE("xenvchan init failed: %s\n", strerror(errno)); // FIXME is errno meaningful?
    return NULL;
  }
  vchan->blocking = 1;

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("malloc failed: %s\n", strerror(errno));
    return NULL;
  }
  cnx->vchan = vchan;

#ifndef GLS_SERVER
  // wait for connection from server
  LOGI("waiting for vchan establishment...\n");
  while (libxenvchan_is_open(vchan) == 2)
    libxenvchan_wait(vchan);
  assert (libxenvchan_is_open(vchan) == 1);
#endif
  LOGI("vchan connected\n");

  return cnx;
}

static struct gls_connection* xenvchan_tport_client_create(const char* server_addr)
{
  LOGI("initializing xenvchan transport\n");
  (void)server_addr;
  return tport_connection_create();
}

static int xenvchan_tport_connection_fd(struct gls_connection* cnx)
{
  return libxenvchan_fd_for_select(cnx->vchan);
}

static ssize_t xenvchan_tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = libxenvchan_write(cnx->vchan, buffer, size);
  if (ret < 0)
    LOGE("libxenvchan_write failed: %s\n", strerror(errno));
  return ret;
}

static ssize_t xenvchan_tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt)
{
  int i;
  size_t written = 0;
  for (i = 0; i < iovcnt; i++) {
    ssize_t ret = xenvchan_tport_write(cnx, iov[i].iov_base, iov[i].iov_len);
    if (ret < 0) {
      LOGE("libxenvchan_write failed for writev: %s\n", strerror(errno));
      return ret;
    }
    written += ret;
    if ((size_t)ret < iov[i].iov_len)
      break;
  }
  return written;
}

static ssize_t xenvchan_tport_read(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = libxenvchan_read(cnx->vchan, buffer, size);
  if (ret < 0)
    LOGE("libxenvchan_read failed: %s\n", strerror(errno));
  return ret;
}

static void xenvchan_tport_close(struct gls_connection* cnx)
{
  libxenvchan_close(cnx->vchan);
}

struct gls_transport gls_tport_xenvchan = {
  .name = "xenvchan",
  .connection_create = xenvchan_tport_connection_create,
  .client_create = xenvchan_tport_client_create,
  .connection_fd = xenvchan_tport_connection_fd,
  .write = xenvchan_tport_write,
  .writev = xenvchan_tport_writev,
  .read = xenvchan_tport_read,
  .close = xenvchan_tport_close,
};
