#include "ltt_wrappers.h"

#if USE_LTT

# define TRACEPOINT_CREATE_PROBES
# define TRACEPOINT_DEFINE
# include <lttng/tracepoint.h>

void init_lttng_tracepoints(void)
{
  __tracepoints__init();
  __tracepoints__ptrs_init();
  init_lttng_tracepoints_api();
  init_lttng_tracepoints_ring();
  init_lttng_tracepoints_tport();
}

#else

void init_lttng_tracepoints(void)
{
}

#endif
