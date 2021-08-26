# gl-streaming internals

This document attempts to describe the structure of the code, mostly
inherited from previous projects.  It is surely incomplete, possibly
not 100% accurate, and subject to change, hopefully to make the system
better.


## basic high-level structure

1. Application makes EGL/GLES2 API calls, implemented by the
   [clientegl](../gl_client/clientegl.c) and
   [clientgles](../gl_client/clientgles.c) modules

2. API calls are marshalled into messages, described in
   [gls_command.h](../common/gls_command.h), sent using TCP, and possibly
   wait for a reply before the API returns control to the application

3. Server initializes GLES2 using EGL and creates a static window on
   startup, in [glcontrol](../gl_server/glcontrol.c) (this will have to
   change for better window management)

4. Server receives packets with [server](../common/recvr.c) in a
   dedicated thread, managed from [glserver](../gl_server/glserver.c) and
   puts them in a [FIFO queue](../common/fifo.h)

5. A server thread from [glserver](../gl_server/glserver.c) executes
   commands from the FIFO, with implementations from
   [serveregl](../gl_server/serveregl.c) and
   [servergles](../gl_server/servergles.c) modules

6. Serverside implementations for calls with output parameters
   marshall a message like on client side and sends it back; client
   receives them from its own [server](../common/server.c) thread, and
   received synchronously


## data handling

### emission

Client-side API implementations are essentially forwarding the call
parameters to the server (and receiving output data if any).

Every command message contains a command identifier, defined in
[gls_command.h](../common/gls_command.h), which in most cases uniquely
map to an API call.

Small fixed-sized input parameters are filled into a per-API-call data
structure, located in `out_buf` buffer provided to the client-side
implementations as temporary space for message composition before
sending.

Larger input data is sent with a `SEND_DATA` message pior to the
command message itself.

When a client starts, it sends a special `HANDSHAKE` message to check
the server's protocol version, and get the size of the server-created
window.

### reception

#### queuing to FIFO

To avoid dynamic allocation, a fixed-size FIFO of fixed-size packet
buffers is pre-allocated.  Data received from the network lands here
as long as there is space in the FIFO.  It may be necessary to adjust
the compile-time `FIFO_SIZE_ORDER` if "FIFO full" is reported (causing
throttling of network reception).

Incoming `SEND_DATA` messages too large for a FIFO packet get a
newly-allocated buffer just for them.  They still use a FIFO packet
preserving the order of arrival, and it contains the
`gls_cmd_send_data_t` header, including a pointer to the allocated
data (a temporary hack which brings a nasty `zero` field to the
protocol just to reserve space for that pointer).

#### dequeuing and execution

When dequeuing an API command message from the FIFO, it gets passed by
pointer to the API implementation.

`SEND_DATA` message are handled according to where they were stored:

* those small enough to fit in a FIFO buffer (`dataptr == NULL`)are
  copied in `tmp_buf` to free their FIFO slot * those

* the larger ones get their `dataptr` stored in `pool.mallocated`
  alongside the (unused) `tmp_buf`; that points to the `SEND_DATA`
  header, and we need it for `free()` later, and the data encapsulated
  within it is stored a ref in `pool.data_payload`

API commands expecting input from a `SEND_DATA` message just use it
from there, according to the whether `pool.mallocated` is `NULL` or
holds a pointer (in which case it is finally freed after use).

### output from API commands

A message with output parameters uses has its client-side
implementation use `wait_for_data()`, which expects to receive a
`SEND_DATA` message in client FIFO, and subsequently reads those
results from `tmp_buf` just like the server does with large inputs.

### batched commands

Upstream used a batching mechanism to send several API calls in a
single UDP packet.  This has been culled from the current codebase
as premature optimisation and because of inherent limitations (see [older
INTERNALS](https://github.com/ydirson/gl-streaming/blob/346d1b58e6e3a220c7cae28c3afce02cf2fdd83f/INTERNALS.md)).


## special client work

On client side, we try to keep the code minimal, just piping data to
the server.  However, not all API calls are simple enough for this to
be possible.  Some exceptions are:

### `eglQueryString` and `glGetString`

The strings a client app can retrieve with those calls are specified
to be static strings.  Forwarding each call to the server would not
only be inefficient (which is not a problem in itself, as this ought
not to be a frequently-repeated call), but also raises memory
management issues.

For `eglQueryString` we have to wait until a valid `EGLDisplay` is
passed to fetch any string other than client-extension.  However since
the returned strings have to be static, we cannot even afford a
`realloc()` call after a single API call.  We thus have to store
client-extension in a separate storage buffer, and it will be queried
and cached on first call.  All others, display-dependant, will be all
queried and stored the first time any of them is requested.  All
further requests will hit the cache.

The `glGetString` case is simpler but has the same "static string"
requirement; we can just fetch all valid strings to populate the cache
on first call.

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
commands suffering this loss will not be executed as they will miss
input.

Also, usage of oversized arrays in non-`SEND_DATA` "just to be sure we
don't miss something" is still too common today.

As well, a number of calls don't properly check the size of data
blocks will fit in the space reserved for them.

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
