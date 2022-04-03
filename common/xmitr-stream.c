#include "xmitr.h"
#include "transport.h"
#include "fastlog.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define GLS_OUT_BUFFER_SIZE 4096 // 2048

struct xmitr {
  struct gls_connection* cnx;
  char* outbuf;
  size_t outbufsize;
};

// FIXME malloc instead
static struct xmitr the_xmitr;

struct xmitr* xmitr_init(struct gls_connection* cnx)
{
  assert(!the_xmitr.outbuf); // already init'd
  the_xmitr.cnx = cnx;
  the_xmitr.outbuf = (char*)malloc(GLS_OUT_BUFFER_SIZE);
  if (the_xmitr.outbuf == NULL) {
    LOGE("failed to allocate xmitr out_buf: %s\n", strerror(errno));
    return NULL;
  }
  the_xmitr.outbufsize = GLS_OUT_BUFFER_SIZE;
  return &the_xmitr;
}

void xmitr_free(struct xmitr* xmitr)
{
  free(xmitr->outbuf);
  xmitr->outbuf = NULL;
}

void* xmitr_getbuf(struct xmitr* xmitr)
{
  assert(xmitr->outbuf);
  return xmitr->outbuf;
}

size_t xmitr_getbufsize(struct xmitr* xmitr)
{
  assert(xmitr->outbuf);
  return xmitr->outbufsize;
}

int xmitr_sendbuf(struct xmitr* xmitr, size_t size)
{
  if (tport_write(xmitr->cnx, xmitr->outbuf, size) < 0) {
    tport_close(xmitr->cnx);
    return -1;
  }
  return 0;
}
