# gl-streaming internals

This document attempts to describe the structure of the code, mostly
inherited from previous projects.  It is surely incomplete, possibly
not 100% accurate, and subject to change, hopefully to make the system
better.


## basic high-level structure

1. Application makes EGL/GLES2 API calls, implemented by the
   [clientegl](gl_client/clientegl.c) and
   [clientgles](gl_client/clientgles.c) modules

2. API calls are marshalled into messages, described in
   [gls_command.h](common/gls_command.h), and sent using TCP, and
   possibly wait for a reply before the API returns control to the
   application

3. Server initializes GLES2 using EGL and creates a static window on
   startup, in [glcontrol](gl_server/glcontrol.c) (this will have to
   change for better window management)

4. Server receives packets with [server](common/server.c) in a
   dedicated thread, managed from [glserver](gl_server/glserver.c) and
   puts them in a [FIFO queue](common/fifo.h)

5. A server thread from [glserver](gl_server/glserver.c) executes
   commands from the FIFO, with implementations from
   [serveregl](gl_server/serveregl.c) and
   [servergles](gl_server/servergles.c) modules

6. Serverside implementations for calls with output parameters
   marshall a message like on client side and sends it back; client
   receives them from its own [server](common/server.c) thread, and
   received synchronously


## buffers and transfer modes

The networking code uses 2 buffers:

* `out_buf` to compose an outgoing fixed-size message
* `tmp_buf` to:
  * send large variable-length data
  * receive results from server
  * batch simpler calls which do not need tmp_buf for any usage listed
    above

A message with variable-length data gets sent as 2 messages:

* first the data in a `SEND_DATA` message, using `gls_cmd_send_data`
* then the command with its fixed arguments in a command-specific
  structure; on execution on the server it finds in data right in
  `tmp_buf`

A message with output parameters uses `wait_for_data`, and
subsequently finds those results in `tmp_buf`.

A message that does not need `tmp_buf` for those usages is accumulated
into `tmp_buf` using `GLS_PUSH_BATCH`, until one of those conditions
occur:

* a given volume of messages get queued (see `BATCH_AUTO_FLUSH_SIZE`)
* an API function with a need for `tmp_buf` is called

In either case, a `BREAK` message is queued in `tmp_buf` which is then
sent as `SEND_DATA`, and a `FLUSH` message is sent to trigger their
execution.


# other things to be documented

* other GLS internal messages: `FLIP`, `get_context`
* more protocol details
