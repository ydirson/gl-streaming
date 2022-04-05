#include "xmitr.h"
#include "transport.h"
#include "fastlog.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define GLS_OUT_BUFFER_SIZE 4096 // 2048

struct xmitr_stream {
  struct xmitr xmitr;
  // above field must be first
  struct gls_connection* cnx;
  char* outbuf;
  size_t outbufsize;
};

static void xmitr_stream_free(struct xmitr* xmitr)
{
  struct xmitr_stream* xmitr_stream = (struct xmitr_stream*)xmitr;
  free(xmitr_stream->outbuf);
  xmitr_stream->outbuf = NULL;
}

static void* xmitr_stream_getbuf(struct xmitr* xmitr)
{
  struct xmitr_stream* xmitr_stream = (struct xmitr_stream*)xmitr;
  assert(xmitr_stream->outbuf);
  return xmitr_stream->outbuf;
}

static size_t xmitr_stream_getbufsize(struct xmitr* xmitr)
{
  struct xmitr_stream* xmitr_stream = (struct xmitr_stream*)xmitr;
  assert(xmitr_stream->outbuf);
  return xmitr_stream->outbufsize;
}

static int xmitr_stream_sendbuf(struct xmitr* xmitr, size_t size)
{
  struct xmitr_stream* xmitr_stream = (struct xmitr_stream*)xmitr;
  if (tport_write(xmitr_stream->cnx, xmitr_stream->outbuf, size) < 0) {
    tport_close(xmitr_stream->cnx);
    return -1;
  }
  return 0;
}

// FIXME
#include "gls_command.h"

static int xmitr_stream_senddata(struct xmitr* xmitr, const void* data, size_t size)
{
  struct xmitr_stream* xmitr_stream = (struct xmitr_stream*)xmitr;
  char* out_buf = xmitr_getbuf(xmitr);
  gls_cmd_send_data_t* c = (gls_cmd_send_data_t*)out_buf;
  c->cmd = GLSC_SEND_DATA;
  c->cmd_size = sizeof(gls_cmd_send_data_t) + size;
  c->zero = 0;

  struct iovec iov[] = {
    { c, sizeof(gls_cmd_send_data_t) },
    { (void*)data, size }
  };

  if (tport_writev(xmitr_stream->cnx, iov, sizeof(iov)/sizeof(iov[0])) < 0) {
    tport_close(xmitr_stream->cnx);
    return -1;
  }
  return 0;
}

//

static struct xmitr_ops xmitr_stream_ops = {
  .getbuf = xmitr_stream_getbuf,
  .getbufsize = xmitr_stream_getbufsize,
  .sendbuf = xmitr_stream_sendbuf,
  .senddata = xmitr_stream_senddata,
  .free = xmitr_stream_free,
};

// FIXME malloc instead
static struct xmitr_stream the_xmitr;

struct xmitr* xmitr_stream_init(struct gls_connection* cnx)
{
  assert(!the_xmitr.outbuf); // already init'd
  the_xmitr.xmitr.ops = &xmitr_stream_ops;
  the_xmitr.cnx = cnx;
  the_xmitr.outbuf = (char*)malloc(GLS_OUT_BUFFER_SIZE);
  if (the_xmitr.outbuf == NULL) {
    LOGE("failed to allocate xmitr out_buf: %s\n", strerror(errno));
    return NULL;
  }
  the_xmitr.outbufsize = GLS_OUT_BUFFER_SIZE;
  return &the_xmitr.xmitr;
}
