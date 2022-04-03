#include "xmitr.h"
#include "fastlog.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define GLS_OUT_BUFFER_SIZE 4096 // 2048

struct xmitr {
  char* outbuf;
  size_t outbufsize;
};

// FIXME malloc instead
static struct xmitr the_xmitr;

struct xmitr* xmitr_init(void)
{
  assert(!the_xmitr.outbuf); // already init'd
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
