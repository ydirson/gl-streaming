// LTTng tracepoint provider for GLS/EGL/GLES/etc API tracing

#if USE_LTT

# undef TRACEPOINT_PROVIDER
# define TRACEPOINT_PROVIDER gls_api

# undef TRACEPOINT_INCLUDE
# define TRACEPOINT_INCLUDE "./ltt_api_tp.h"

# if !defined(LTT_API_TP_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#  define LTT_API_TP_H_

#  include <lttng/tracepoint.h>

#  include "gls_command.h"

TRACEPOINT_EVENT_CLASS(gls_api, gls_call_eventclass,
                       TP_ARGS(enum GL_Server_Command, cmd),
                       TP_FIELDS(ctf_integer_hex(enum GL_Server_Command, cmd, cmd)
                                 )
                       )

TRACEPOINT_EVENT_INSTANCE(gls_api, gls_call_eventclass, call,
                          TP_ARGS(enum GL_Server_Command, cmd)
                          )
TRACEPOINT_EVENT_INSTANCE(gls_api, gls_call_eventclass, calldone,
                          TP_ARGS(enum GL_Server_Command, cmd)
                          )

# endif

# include <lttng/tracepoint-event.h>

#else

# define tracepoint(...) do {} while(0)

#endif // USE_LTT
