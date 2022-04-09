#pragma once

// pollable notifications

#include <stdint.h>

struct notifier;
static inline int notifier_init(struct notifier* notifier);
static inline void notifier_close(struct notifier* notifier);
static inline int notifier_terminate(struct notifier* notifier);
static inline int notify(struct notifier* notifier);
static inline uint64_t notifier_drain(struct notifier* notifier);
static inline int notifier_fd(struct notifier* notifier);

#if 1
// eventfd implementation

#include <sys/eventfd.h>
#include <unistd.h>

struct notifier {
  int event_fd;
  int quit;
};

static inline int notifier_init(struct notifier* notifier)
{
  int ret = eventfd(0, EFD_CLOEXEC);
  if (ret < 0)
    return ret;
  notifier->event_fd = ret;
  notifier->quit = 0;
  return 0;
}

static inline int notifier_init_fromfd(struct notifier* notifier, int fd)
{
  LOGD("notifier_init_fromfd(%d)\n", fd);
  notifier->event_fd = fd;
  notifier->quit = 0;
  return 0;
}

static inline void notifier_close(struct notifier* notifier)
{
  close(notifier->event_fd);
  notifier->event_fd = -1;
}

static inline int notifier_terminate(struct notifier* notifier)
{
  notifier->quit = 1;
  return notify(notifier);
}

static inline int notifier_has_terminated(struct notifier* notifier)
{
  return notifier->quit;
}

static inline int notify(struct notifier* notifier)
{
  uint64_t val = 1;
  return write(notifier->event_fd, &val, sizeof(val));
}

static inline uint64_t notifier_drain(struct notifier* notifier)
{
  uint64_t val;
  int ret = read(notifier->event_fd, &val, sizeof(val));
  if (ret < 0)
    return 0;
  return val;
}

static inline int notifier_fd(struct notifier* notifier)
{
  return notifier->event_fd;
}

#else
// pipe implementation

#ifndef _GNU_SOURCE
# error needs _GNU_SOURCE
#endif

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

struct notifier {
  int pipe_wr, pipe_rd;
};

static inline int notifier_init(struct notifier* notifier)
{
  int pipefds[2];
  int ret = pipe2(pipefds, O_CLOEXEC);
  if (ret < 0)
    return ret;
  notifier->pipe_rd = pipefds[0];
  notifier->pipe_wr = pipefds[1];
  return 0;
}

static inline int notifier_init_fromfd(struct notifier* notifier, int fd)
{
  notifier->pipe_rd = fd;
  notifier->pipe_wr = -1;
  return 0;
}

static inline void notifier_close(struct notifier* notifier)
{
  close(notifier->pipe_wr);
  notifier->pipe_wr = -1;
  close(notifier->pipe_rd);
  notifier->pipe_rd = -1;
}

static inline int notifier_terminate(struct notifier* notifier)
{
  int ret = close(notifier->pipe_wr);
  if (ret < 0)
    return ret;
  notifier->pipe_wr = -1;
  return ret;
}

static inline int notifier_has_terminated(struct notifier* notifier)
{
  return 0; // detected by poll as EOF on fd
}

static inline int notify(struct notifier* notifier)
{
  return write(notifier->pipe_wr, "", 1);
}

static inline uint64_t notifier_drain(struct notifier* notifier)
{
  char buf;
  int ret = read(notifier->pipe_rd, &buf, 1);
  if (ret < 0)
    return 0;
  return 1;
}

static inline int notifier_fd(struct notifier* notifier)
{
  return notifier->pipe_rd;
}

#endif
