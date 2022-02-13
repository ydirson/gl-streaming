#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/uio.h>

// GLS Transport API

struct gls_server;
struct gls_connection;

struct gls_server* tport_server_create(const char* listen_addr, uint16_t listen_port);
struct gls_connection* tport_client_create(const char* server_addr);
struct gls_connection* tport_server_wait_connection(struct gls_server* srv);
int tport_connection_fd(struct gls_connection*);
void tport_close(struct gls_connection* cnx);

ssize_t tport_write(struct gls_connection* cnx, void* buffer, size_t size);
ssize_t tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt);
ssize_t tport_read(struct gls_connection* cnx, void* buffer, size_t size);
int tport_peek(struct gls_connection* cnx, void* buffer, size_t bufsize);
