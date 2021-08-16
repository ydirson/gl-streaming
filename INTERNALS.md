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
   [gls_command.h](common/gls_command.h), and sent using UDP, and
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

* `out_buf` to compose an outgoing messages, whether fixed-size or large
  variable-length data
* `tmp_buf` to:
  * receive results from server
  * batch simpler calls which do not need tmp_buf for any usage listed
    above

A message with large variable-length data gets sent as 2 messages:

* first the data in a `SEND_DATA` message, using `gls_cmd_send_data`
* then the command with its fixed arguments in a command-specific
  structure; on execution on the server it finds in data right in
  `tmp_buf`

A message with output parameters uses `wait_for_data`, and
subsequently finds those results in `tmp_buf`.

When a client starts, it sends a handshake `get_context` message to
check the server's protocol version, and get the size of the
server-created window.

### batched commands

A message that does not need `tmp_buf` for those usages is accumulated
into `tmp_buf` using `GLS_PUSH_BATCH`, until one of those conditions
occur:

* a given volume of messages get queued (see `BATCH_AUTO_FLUSH_SIZE`)
* an API function with a need for `tmp_buf` is called

In either case, a `BREAK` message is queued in `tmp_buf` which is then
sent as `SEND_DATA`, and a `FLUSH` message is sent to trigger their
execution.


## special client work

On client side, we try to keep the code minimal, just piping data to
the server.  However, not all API calls are simple enough for this to
be possible.  Some exceptions are:

### `eglGetError`, `glGetError` and GLS-level errors

Sometimes we can report at GLS client level an error before even
sending a command to the server.  This is currently implemented for
EGL through the `client_egl_error` variable.  It can be set by any
`egl*` function, and gets reset by `send_packet` to avoid hiding later
errors.

For lack of a generic error code, we often use EGL_BAD_ACCESS.

### `glVertexAttribPointer` client-array case

`glVertexAttribPointer` interprets differently its `pointer` parameter,
depending whether a VBO is active (in which case it is an offset into
the VBO) or not (in which case it is a pointer to client array).

The problem is that the client-array call does not specify the array
size, so the client cannot easily transfer the data at this point.
The vertex attributes set this way are used later, eg. by
`glDrawArrays`, and those calls are the ones getting the number of
vertices for which attributes are given... so the data can only be
transferred to the server at that time.  Then we cannot know either,
if the data will be reused and should be kept in server memory for a
later call, so to avoid leaking the server's memory we basically have
to retransfer the data every time it is used (we could also come with
other solutions, like implementing a fixed-sized server cache for
those arrays; it seems unreasonably difficult to find out by ourselves
and give hints to the server, without app's cooperation).

Thus this implementation defers the `glVertexAttribPointer` calls
pointing to client-data, and additionally creates a VBO to hold the
attribute data (this is what upstream calls `GLS_EMULATE_VBO`, has
been cleaned up and is unconditionally activated for lack of an
alternative, though we could implement a no-VBO version).

Some optimisations could be done, eg. send data block only once in the
case of interleaved attributes in the same client array.  Upstream
code did this to some extent (only if attribute 0 was active and with
dubious array-bounds handling), but this has been discarded as
premature optimisation.


Note: the `glBindBuffer` doc says the buffer "name" is "local to the
shared object space of the current GL rendering context".  Since we're
not tracking any context we can assume this will break when more than
one shared object space gets used.

### `glDrawElements` client-array case

This case has yet to be properly handled, and is protected by an
`assert(0)` in the meantime.

### `eglQuerySurface` and size attributes

Currently we use a single (fixed-size) window on server side, created
when launching the server.  This usually does not match the size of
the window created by client apps.  When an app queries the width or
height of the window surface to fit rendering to this size, querying
the server for the real EGLSurface size gives unintended results (and
in the case where a demo app like `es2tri` checks the size matches its
expectations, can result in early abort).


## current implementation shortcuts

### displays, windows

Currently a single window on a single display is supported.  Since the
window creation by cient app is out of the EGL scope, it is today not
intercepted, and gets shown with a background that will stay black.

On server side, a fixed-sized window (today hardcoded with size
1280x720) is created at server startup.

### EGL surfaces

Currently only a single surface is supported, which is the window
surface for the single window.  Only this one is ever returned by any
API call (which makes pixmap surfaces and pbuffer surfaces mostly
unusable, and usually makes their usage break the in-window
rendering).


# other things to be documented

* more protocol details
