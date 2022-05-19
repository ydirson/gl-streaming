// LTTng tracepoint provider for transport tracing

#if USE_LTT

# undef TRACEPOINT_PROVIDER
# define TRACEPOINT_PROVIDER gls_tport

# undef TRACEPOINT_INCLUDE
# define TRACEPOINT_INCLUDE "./ltt_tport_tp.h"

# if !defined(LTT_TPORT_TP_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#  define LTT_TPORT_TP_H_

#  include <lttng/tracepoint.h>

TRACEPOINT_EVENT_CLASS(gls_tport, gls_tport_size_eventclass,
                       TP_ARGS(void*, cnx,
                               size_t, size),
                       TP_FIELDS(ctf_integer_hex(void*, cnx, cnx)
                                 ctf_integer(int, size, size)
                                 )
                       )

TRACEPOINT_EVENT_INSTANCE(gls_tport, gls_tport_size_eventclass,
                          write,
                          TP_ARGS(void*, cnx, size_t, size)
                          )
TRACEPOINT_EVENT_INSTANCE(gls_tport, gls_tport_size_eventclass,
                          writedone,
                          TP_ARGS(void*, cnx, size_t, size)
                          )

TRACEPOINT_EVENT_INSTANCE(gls_tport, gls_tport_size_eventclass,
                          writev,
                          TP_ARGS(void*, cnx, size_t, size)
                          )
TRACEPOINT_EVENT_INSTANCE(gls_tport, gls_tport_size_eventclass,
                          writevdone,
                          TP_ARGS(void*, cnx, size_t, size)
                          )

TRACEPOINT_EVENT_INSTANCE(gls_tport, gls_tport_size_eventclass,
                          read,
                          TP_ARGS(void*, cnx, size_t, size)
                          )
TRACEPOINT_EVENT_INSTANCE(gls_tport, gls_tport_size_eventclass,
                          readdone,
                          TP_ARGS(void*, cnx, size_t, size)
                          )

# endif

# include <lttng/tracepoint-event.h>

#else

# define tracepoint(...) do {} while(0)

#endif // USE_LTT
