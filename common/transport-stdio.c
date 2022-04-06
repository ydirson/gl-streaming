#include "transport.h"
#include "fastlog.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct gls_connection
{
  int read_fd;
  int write_fd;
};

static struct gls_connection* stdio_tport_connection_create(void)
{
  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("malloc failed: %s\n", strerror(errno));
    return NULL;
  }
  cnx->read_fd = STDIN_FILENO;
  cnx->write_fd = STDOUT_FILENO;
  return cnx;
}

static struct gls_connection* stdio_tport_client_create(const char* server_addr)
{
  LOGI("initializing stdio transport\n");
  (void)server_addr;
  return tport_connection_create();
}

static int stdio_tport_connection_fd(struct gls_connection* cnx)
{
  return cnx->read_fd;
}

static ssize_t stdio_tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = write(cnx->write_fd, buffer, size);
  if (ret < 0)
    LOGE("write failed: %s\n", strerror(errno));
  return ret;
}

static ssize_t stdio_tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt)
{
  ssize_t ret = writev(cnx->write_fd, iov, iovcnt);
  if (ret < 0)
    LOGE("writev failed: %s\n", strerror(errno));
  return ret;
}

static ssize_t stdio_tport_read(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = read(cnx->read_fd, buffer, size);
  if (ret < 0)
    LOGE("read failed: %s\n", strerror(errno));
  return ret;
}

static void stdio_tport_close(struct gls_connection* cnx)
{
  close(cnx->write_fd);
  close(cnx->read_fd);
}

struct gls_transport gls_tport_stdio = {
  .name = "stdio",
  .connection_create = stdio_tport_connection_create,
  .client_create = stdio_tport_client_create,
  .connection_fd = stdio_tport_connection_fd,
  .write = stdio_tport_write,
  .writev = stdio_tport_writev,
  .read = stdio_tport_read,
  .close = stdio_tport_close,
};
