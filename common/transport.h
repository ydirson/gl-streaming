#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/uio.h>

// GLS Transport API

struct gls_server;
struct gls_connection;

// transport registration

struct gls_transport
{
  const char* name;

  // API function pointers
  struct gls_server* (*server_create)(const char* server_addr);
  struct gls_connection* (*server_wait_connection)(struct gls_server* srv);

  struct gls_connection* (*client_create)(const char* server_addr);

  int (*connection_fd)(struct gls_connection*);
  void (*close)(struct gls_connection* cnx);

  ssize_t (*write)(struct gls_connection* cnx, void* buffer, size_t size);
  ssize_t (*writev)(struct gls_connection* cnx, struct iovec *iov, int iovcnt);
  ssize_t (*read)(struct gls_connection* cnx, void* buffer, size_t size);
};

extern struct gls_transport gls_tport_tcp;
extern struct gls_transport* the_tport;

int tport_select(const char* name);

// public APIs, as macro wrappers

#define tport_server_create(server_addr) the_tport->server_create(server_addr)
#define tport_server_wait_connection(srv) the_tport->server_wait_connection(srv)

#define tport_client_create(server_addr) the_tport->client_create(server_addr)

#define tport_connection_fd(cnx) the_tport->connection_fd(cnx)
#define tport_close(cnx) the_tport->close(cnx)

#define tport_write(cnx, buffer, size) the_tport->write(cnx, buffer, size)
#define tport_writev(cnx, iov, iovcnt) the_tport->writev(cnx, iov, iovcnt)
#define tport_read(cnx, buffer, size) the_tport->read(cnx, buffer, size)
