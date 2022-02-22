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

int tport_server_create(const char* listen_addr, uint16_t listen_port)
{
  int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (listen_fd < 0) {
    fprintf(stderr, "GLS ERROR: receiver socket open: %s\n", strerror(errno));
    return -1;
  }

  int sockopt = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
    LOGE("GLS ERROR: setsockopt(SO_REUSEADDR) failed: %s\n", strerror(errno));
    return -1;
  }

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(listen_port);
  sai.sin_addr.s_addr = inet_addr(listen_addr);
  if (bind(listen_fd, (struct sockaddr*)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: bind failed: %s\n", strerror(errno));
    return -1;
  }

  if (listen(listen_fd, 1) < 0) {
    LOGE("GLS ERROR: listen failed: %s\n", strerror(errno));
    return -1;
  }

  return listen_fd;
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

int tport_client_create(const char* connect_addr, uint16_t connect_port)
{
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0) {
    fprintf(stderr, "GLS ERROR: receiver socket open: %s\n", strerror(errno));
    return -1;
  }

  if (tcp_socket_setup(fd) < 0)
    return -1;

  struct sockaddr_in sai;
  sai.sin_family = AF_INET;
  sai.sin_port = htons(connect_port);
  sai.sin_addr.s_addr = inet_addr(connect_addr);
  if (connect(fd, (struct sockaddr*)&sai, sizeof(sai)) < 0) {
    LOGE("GLS ERROR: connect failed: %s\n", strerror(errno));
    return -1;
  }

  return fd;
}

int tport_server_wait_connection(int listen_fd, struct sockaddr* addr, socklen_t* addrlen)
{
  *addrlen = sizeof(*addr);
  int fd = accept4(listen_fd, addr, addrlen, SOCK_CLOEXEC);
  if (fd < 0) {
    LOGE("GLS ERROR: server accept: %s\n", strerror(errno));
    return -1;
  }

  if (tcp_socket_setup(fd) < 0)
    return -1;

  return fd;
}

ssize_t tport_read(int fd, void* buffer, size_t bufsize)
{
  ssize_t recv_size = recv(fd, buffer, bufsize, 0);
  if (recv_size < 0)
    LOGE("GLS ERROR: transport socket recv: %s\n", strerror(errno));
  else if (recv_size == 0)
    LOGI("GLS INFO: transport socket closed\n");

  return recv_size;
}
