// LTTng trace provider package

#define TRACEPOINT_CREATE_PROBES
#define TRACEPOINT_DEFINE
#include "ltt_tport_tp.h"

#include "ltt_wrappers.h"

void init_lttng_tracepoints_tport(void)
{
  __lttng_events_init__gls_tport();
}
