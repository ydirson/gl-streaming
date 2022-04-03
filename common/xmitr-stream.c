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

// FIXME
#include "gls_command.h"

int xmitr_senddata(struct xmitr* xmitr, const void* data, size_t size)
{
  char* out_buf = xmitr_getbuf(xmitr);
  gls_cmd_send_data_t* c = (gls_cmd_send_data_t*)out_buf;
  c->cmd = GLSC_SEND_DATA;
  c->cmd_size = sizeof(gls_cmd_send_data_t) + size;
  c->zero = 0;

  struct iovec iov[] = {
    { c, sizeof(gls_cmd_send_data_t) },
    { (void*)data, size }
  };

  if (tport_writev(xmitr->cnx, iov, sizeof(iov)/sizeof(iov[0])) < 0) {
    tport_close(xmitr->cnx);
    return -1;
  }
  return 0;
}
