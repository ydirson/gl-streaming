// LTTng tracepoint provider for ring-buffer tracing

#if USE_LTT

# undef TRACEPOINT_PROVIDER
# define TRACEPOINT_PROVIDER gls_ring

# undef TRACEPOINT_INCLUDE
# define TRACEPOINT_INCLUDE "./ltt_ring_tp.h"

# if !defined(LTT_RING_TP_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#  define LTT_RING_TP_H_

#  include <lttng/tracepoint.h>

TRACEPOINT_EVENT_CLASS(gls_ring, gls_ringindex_eventclass,
                       TP_ARGS(void*, ring,
                               int, idx),
                       TP_FIELDS(ctf_integer_hex(void*, ring, ring)
                                 ctf_integer(int, index, idx)
                                 )
                       )

TRACEPOINT_EVENT_INSTANCE(gls_ring, gls_ringindex_eventclass,
                          push,
                          TP_ARGS(void*, ring, int, idx)
                          )
TRACEPOINT_EVENT_INSTANCE(gls_ring, gls_ringindex_eventclass,
                          pushdone,
                          TP_ARGS(void*, ring, int, idx)
                          )

TRACEPOINT_EVENT_INSTANCE(gls_ring, gls_ringindex_eventclass,
                          pop,
                          TP_ARGS(void*, ring, int, idx)
                          )
TRACEPOINT_EVENT_INSTANCE(gls_ring, gls_ringindex_eventclass,
                          popdone,
                          TP_ARGS(void*, ring, int, idx)
                          )
# endif

# include <lttng/tracepoint-event.h>

#else

# define tracepoint(...) do {} while(0)

#endif // USE_LTT
