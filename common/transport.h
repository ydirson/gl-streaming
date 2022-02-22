#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/uio.h>

#include <sys/socket.h>

// GLS Transport API

int tport_server_create(const char* listen_addr, uint16_t listen_port);
int tport_client_create(const char* connect_addr, uint16_t connect_port);
int tport_server_wait_connection(int listen_fd, struct sockaddr* addr, socklen_t* addrlen);
ssize_t tport_read(int fd, void* buffer, size_t size);
int tport_peek(int fd, void* buffer, size_t readsize);
