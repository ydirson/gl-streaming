#pragma once
#include "ring.h"
#include <stddef.h>

struct xmitr;
struct gls_connection;

struct xmitr_ops {
  void* (*getbuf)(struct xmitr*);
  size_t (*getbufsize)(struct xmitr*);
  int (*sendbuf)(struct xmitr*, size_t);
  int (*senddata)(struct xmitr* xmitr, const void* data, size_t size);
  void (*free)(struct xmitr*);
};

struct xmitr {
  struct xmitr_ops* ops;
};

struct xmitr* xmitr_stream_init(struct gls_connection*);
struct xmitr* xmitr_shm_init(ring_allocator_t*, void*);

static inline void* xmitr_getbuf(struct xmitr* xmitr) {
  return xmitr->ops->getbuf(xmitr);
}
static inline size_t xmitr_getbufsize(struct xmitr* xmitr) {
  return xmitr->ops->getbufsize(xmitr);
}
static inline int xmitr_sendbuf(struct xmitr* xmitr, size_t size) {
  return xmitr->ops->sendbuf(xmitr, size);
}
static inline int xmitr_senddata(struct xmitr* xmitr, const void* data, size_t size) {
  return xmitr->ops->senddata(xmitr, data, size);
}
static inline void xmitr_free(struct xmitr* xmitr) {
  xmitr->ops->free(xmitr);
}
