#pragma once

// pollable notifications

struct notifier;
static inline int notifier_init(struct notifier* notifier);
static inline void notifier_close(struct notifier* notifier);
static inline int notifier_terminate(struct notifier* notifier);
static inline int notify(struct notifier* notifier);
static inline int notifier_drain_one(struct notifier* notifier);
static inline int notifier_fd(struct notifier* notifier);

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

static inline int notify(struct notifier* notifier)
{
  return write(notifier->pipe_wr, "", 1);
}

static inline int notifier_drain_one(struct notifier* notifier)
{
  char buf;
  return read(notifier->pipe_rd, &buf, 1);
}

static inline int notifier_fd(struct notifier* notifier)
{
  return notifier->pipe_rd;
}
