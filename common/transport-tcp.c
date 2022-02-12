#define _GNU_SOURCE
#include "transport.h"
#include "fastlog.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
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
  struct
  {
    struct sockaddr addr;
    socklen_t addrlen;
  } peer;
  int sock_fd;
};

struct gls_server* tport_server_create(const char* listen_addr, uint16_t listen_port)
{
  int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (listen_fd < 0) {
    LOGE("GLS ERROR: receiver socket open: %s\n", strerror(errno));
    return NULL;
  }

  int sockopt = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
    LOGE("GLS ERROR: setsockopt(SO_REUSEADDR) failed: %s\n", strerror(errno));
    return NULL;
  }

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(listen_port);
  sai.sin_addr.s_addr = inet_addr(listen_addr);
  if (bind(listen_fd, (struct sockaddr*)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: bind failed: %s\n", strerror(errno));
    return NULL;
  }

  if (listen(listen_fd, 1) < 0) {
    LOGE("GLS ERROR: listen failed: %s\n", strerror(errno));
    return NULL;
  }

  struct gls_server* srv = malloc(sizeof(struct gls_server));
  if (!srv) {
    LOGE("GLS ERROR: malloc failed: %s\n", strerror(errno));
    return NULL;
  }

  srv->listen_fd = listen_fd;
  return srv;
}

static int tcp_socket_setup(int fd)
{
  int sockopt = 1;
  if (setsockopt(fd, SOL_TCP, TCP_NODELAY, &sockopt, sizeof(sockopt)) < 0) {
    LOGE("GLS ERROR: setsockopt(TCP_NODELAY) error: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

struct gls_connection* tport_client_create(const char* connect_addr, uint16_t connect_port)
{
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0) {
    LOGE("GLS ERROR: receiver socket open: %s\n", strerror(errno));
    return NULL;
  }

  if (tcp_socket_setup(fd) < 0)
    return NULL;

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(connect_port);
  sai.sin_addr.s_addr = inet_addr(connect_addr);
  if (connect(fd, (struct sockaddr*)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: connect failed: %s\n", strerror(errno));
    return NULL;
  }

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("GLS ERROR: malloc failed: %s\n", strerror(errno));
    return NULL;
  }

  cnx->sock_fd = fd;
  return cnx;
}

struct gls_connection* tport_server_wait_connection(struct gls_server* srv)
{
  struct
  {
    struct sockaddr addr;
    socklen_t addrlen;
  } peer;

  peer.addrlen = sizeof(peer.addr);
  int fd = accept4(srv->listen_fd, &peer.addr, &peer.addrlen, SOCK_CLOEXEC);
  if (fd < 0) {
    LOGE("GLS ERROR: server accept: %s\n", strerror(errno));
    return NULL;
  }

  if (tcp_socket_setup(fd) < 0)
    return NULL;

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("GLS ERROR: malloc failed: %s\n", strerror(errno));
    return NULL;
  }

  cnx->peer.addr = peer.addr;
  cnx->peer.addrlen = peer.addrlen;
  cnx->sock_fd = fd;
  return cnx;
}

int tport_connection_fd(struct gls_connection* cnx)
{
  return cnx->sock_fd;
}

ssize_t tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = send(cnx->sock_fd, buffer, size, 0);
  if (ret < 0)
    LOGE("GLS ERROR: send_packet(%zu) failure: %s\n", size, strerror(errno));

  return ret;
}

ssize_t tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt)
{
  struct msghdr msg = { .msg_iov = iov, .msg_iovlen = iovcnt };

  ssize_t ret = sendmsg(cnx->sock_fd, &msg, 0);
  if (ret < 0)
    LOGE("GLS ERROR: tcp_writev failure: %s\n", strerror(errno));

  return ret;
}

ssize_t tport_read(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t recv_size = recv(cnx->sock_fd, buffer, size, 0);
  if (recv_size < 0)
    LOGE("GLS ERROR: transport socket recv: %s\n", strerror(errno));
  else if (recv_size == 0)
    LOGI("GLS INFO: transport socket closed\n");

  return recv_size;
}

void tport_close(struct gls_connection* cnx)
{
  close(cnx->sock_fd);
}
