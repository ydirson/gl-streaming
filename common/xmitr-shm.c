#include "xmitr.h"
#include "transport.h"
#include "fastlog.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define GLS_OUT_BUFFER_SIZE 4096 // 2048

struct xmitr_shm {
  struct xmitr xmitr;
  // above field must be first
  ring_t ring;
};

static void xmitr_shm_free(struct xmitr* xmitr)
{
  struct xmitr_shm* xmitr_shm = (struct xmitr_shm*)xmitr;
  (void)xmitr_shm;
}

static void* xmitr_shm_getbuf(struct xmitr* xmitr)
{
  struct xmitr_shm* xmitr_shm = (struct xmitr_shm*)xmitr;
  return ring_push_ptr_get(&xmitr_shm->ring);
}

static size_t xmitr_shm_getbufsize(struct xmitr* xmitr)
{
  struct xmitr_shm* xmitr_shm = (struct xmitr_shm*)xmitr;
  return ring_contigfreespace(&xmitr_shm->ring);
}

static int xmitr_shm_sendbuf(struct xmitr* xmitr, size_t size)
{
  struct xmitr_shm* xmitr_shm = (struct xmitr_shm*)xmitr;
  (void)size; assert(size <= ring_contigfreespace(&xmitr_shm->ring));
  ring_push_ptr_next(&xmitr_shm->ring);
  return 1;
}

// FIXME
#include "gls_command.h"

// FIXME sends large data as fragments, bad for zerocopy
static int xmitr_shm_senddata(struct xmitr* xmitr, const void* data, size_t size)
{
  const size_t max_payload_size = xmitr_shm_getbufsize(xmitr) - sizeof(gls_cmd_send_data_t);
  size_t offset;
  for (offset = 0; offset < size; offset += max_payload_size) {
    size_t this_fragment_size = (size - offset < max_payload_size) ? size - offset : max_payload_size;
    gls_cmd_send_data_fragment_t* c = xmitr_shm_getbuf(xmitr);
    c->cmd = GLSC_SEND_DATA_FRAGMENT;
    c->cmd_size = sizeof(gls_cmd_send_data_t) + this_fragment_size;
    c->totalsize = size;
    c->offset = offset;
    memcpy(c->data, data + offset, this_fragment_size);

    if (xmitr_shm_sendbuf(xmitr, xmitr_shm_getbufsize(xmitr)) < 0)
      return -1;
  }
  return 0;
}

//

static struct xmitr_ops xmitr_shm_ops = {
  .getbuf = xmitr_shm_getbuf,
  .getbufsize = xmitr_shm_getbufsize,
  .sendbuf = xmitr_shm_sendbuf,
  .senddata = xmitr_shm_senddata,
  .free = xmitr_shm_free,
};

// FIXME malloc instead
static struct xmitr_shm the_xmitr;

struct xmitr* xmitr_shm_init(ring_allocator_t* allocator, void* allocator_data)
{
  the_xmitr.xmitr.ops = &xmitr_shm_ops;

  if (ring_init(&the_xmitr.ring, -1, allocator, allocator_data, CLT2SRV_API_RING_SIZE_ORDER) < 0) {
    LOGE("failed to init API ring\n");
    return NULL;
  }

  return &the_xmitr.xmitr;
}
