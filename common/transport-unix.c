#define _GNU_SOURCE
#include "transport.h"
#include "fastlog.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct gls_server
{
  int listen_fd;
};

struct gls_connection
{
  int sock_fd;
};

// unix sockets

static void unix_parse_address(const char* addr, const char** host)
{
  static const char* DEFAULT_ADDR = "./gls.sock";
  if (addr == NULL)
    *host = DEFAULT_ADDR;
  else
    *host = addr;
}

static struct gls_server* unix_tport_server_create(const char* server_addr)
{
  const char* listen_addr;
  unix_parse_address(server_addr, &listen_addr);

  int listen_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (listen_fd < 0) {
    LOGE("receiver socket open: %s\n", strerror(errno));
    return NULL;
  }

  struct sockaddr_un sau = { .sun_family = AF_UNIX };
  if (strlen(listen_addr) > sizeof(sau.sun_path) - 1) {
    LOGE("server address path too long\n");
    close(listen_fd);
    return NULL;
  }
  strcpy(sau.sun_path, listen_addr);
  if (bind(listen_fd, (struct sockaddr*)&sau, sizeof(sau)) < 0) {
    LOGE("bind failed: %s\n", strerror(errno));
    close(listen_fd);
    return NULL;
  }

  if (listen(listen_fd, 1) < 0) {
    LOGE("listen failed: %s\n", strerror(errno));
    close(listen_fd);
    return NULL;
  }

  struct gls_server* srv = malloc(sizeof(struct gls_server));
  if (!srv) {
    LOGE("malloc failed: %s\n", strerror(errno));
    close(listen_fd);
    return NULL;
  }

  srv->listen_fd = listen_fd;
  return srv;
}

static struct gls_connection* unix_tport_server_wait_connection(struct gls_server* srv)
{
  int fd = accept(srv->listen_fd, NULL, NULL);
  if (fd < 0) {
    LOGE("server accept: %s\n", strerror(errno));
    return NULL;
  }

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("malloc failed: %s\n", strerror(errno));
    close(fd);
    return NULL;
  }

  cnx->sock_fd = fd;
  return cnx;
}

static struct gls_connection* unix_tport_client_create(const char* server_addr)
{
  LOGI("initializing UNIX socket transport\n");

  const char* connect_addr;
  unix_parse_address(server_addr, &connect_addr);

  int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0) {
    LOGE("receiver socket open: %s\n", strerror(errno));
    return NULL;
  }

  struct sockaddr_un sau = { .sun_family = AF_UNIX };
  if (strlen(connect_addr) > sizeof(sau.sun_path) - 1) {
    LOGE("server address path too long\n");
    close(fd);
    return NULL;
  }
  strcpy(sau.sun_path, connect_addr);
  if (connect(fd, (struct sockaddr*)&sau, sizeof(sau)) < 0) {
    LOGE("bind failed: %s\n", strerror(errno));
    close(fd);
    return NULL;
  }

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("malloc failed: %s\n", strerror(errno));
    close(fd);
    return NULL;
  }

  cnx->sock_fd = fd;
  return cnx;
}


static int unix_tport_connection_fd(struct gls_connection* cnx)
{
  return cnx->sock_fd;
}

static ssize_t unix_tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = send(cnx->sock_fd, buffer, size, 0);
  if (ret < 0)
    LOGE("unix_write(%zu) failure: %s\n", size, strerror(errno));

  return ret;
}

static ssize_t unix_tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt)
{
  struct msghdr msg = { .msg_iov = iov, .msg_iovlen = iovcnt };

  ssize_t ret = sendmsg(cnx->sock_fd, &msg, 0);
  if (ret < 0)
    LOGE("unix_writev failure: %s\n", strerror(errno));

  return ret;
}

static ssize_t unix_tport_read(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t recv_size = recv(cnx->sock_fd, buffer, size, 0);
  if (recv_size < 0)
    LOGE("transport socket recv: %s\n", strerror(errno));
  else if (recv_size == 0)
    LOGI("transport socket closed\n");

  return recv_size;
}

static void unix_tport_close(struct gls_connection* cnx)
{
  if (!cnx)
    return;
  close(cnx->sock_fd);
}

struct gls_transport gls_tport_unix = {
  .name = "unix",
  .server_create = unix_tport_server_create,
  .client_create = unix_tport_client_create,
  .server_wait_connection = unix_tport_server_wait_connection,
  .connection_fd = unix_tport_connection_fd,
  .write = unix_tport_write,
  .writev = unix_tport_writev,
  .read = unix_tport_read,
  .close = unix_tport_close,
};
