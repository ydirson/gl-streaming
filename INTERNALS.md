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


## special client work still to be done

### `eglCreatePixmapSurface`

This gets a client-side Pixmap and, which needs to be transfered ot
the server first.  Needs a custom data struct and a WARN_INEFFICIENT,
or shared memory.


## current implementation shortcuts

### single GL session

The server app does the allocations on behalf of all clients.  Some
consequences are:

- GPU resources not released explicitly by the client (eg. after a
  crash, or simply relying on process exiting to release) stay
  allocated until the server terminates
- resources allocated by previous clients can be seen by later ones,
  causing reproducibility issues

This can be addressed by forking a dedicated server process for each
client connecting, to replicate the client lifetime on the server-side
GL client.  Additionally this will bring a more traditionnal design
where several clients can connect at the same time.

### large buffers

Large data buffers cause problems to the fixed-size tmp_buf.
Currently the code drops data that does not fit in the buffer, and the
commands suffering this loss probably don't know and use undefined
memory following the buffer instead.

Also, usage of oversized buffers "just to be sure we don't miss
something" is just too common today.

### transport reliability

Transport is done with UDP today.  This has nice properties for a
packet-based protocol, but does not provide any reliability features,
which are supposed to be added at application level.  Currently we
don't add any such reliability feature, and are bound to suffer from
packet loss.

Some options:

- add reliability features (tracking packets to get lost ones resent,
  congestion control to limit occurences of losses, etc): this would
  be good if we could drop packets to resync (eg. video), but here we
  do need reliability foremost, and will have no benefit implementing
  that atop UDP
- use a reliable packet transport -- SCTP would work but may be overkill
  just to get reliability, esp. given its relatively low deployment
- implement a packet protocol inside TCP: this will be the easiest and
  most portable.  It will have an impact on batching, though.

### shared memory

Some calls are complicated by the server lacking direct memory access
to the client's.  Examples include `glVertexAttribPointer` as
mentionned above, or `eglCreatePixmapSurface`, which can be emulated
at a cost.  Explicit memory mapping like the `GL_OES_mapbuffer`
extension bring this to another level, which can probably not be
efficiently implemented by a network transport.

However, in the case of a virtual machine, we can study the
implications of establishing real memory sharing.

### displays, windows

Currently a single window on a single display is supported.  Since the
window creation by client app is out of the EGL scope, it is today not
intercepted, and gets shown with a background that will stay black.

On server side, a fixed-sized window (today hardcoded with size
1280x720) is created at server startup.

For transparent usage, we will need to intercept the window creation
to prevent it to happen locally and get the events on the server
window propagated back.  This seems hardly reasonable for
manually-created X11 windows, but can be studied for common
window-handling frameworks like GLFW and SDL2, which would then be
taught a new backend alongside X11.


# other things to be documented

* more protocol details
