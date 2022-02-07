# Build and setup streaming environment

No installation is required to test the software.

To build from source you will need [`meson`](https://mesonbuild.com/)
and [`ninja`](https://ninja-build.org/), both commonly found on
current Linux distros:

```
meson . build/
ninja -C build
```

# Launch server

```
build/gl_server/gl_server
```

Optionally, use `-t` flag to select a non-default transport, and `-s`
flag to select a non-default server address (see below).

The `-n` flag starts a TCP server in "no fork" mode: instead of
forking a separate process to handle an incoming connection, and then
getting ready to handle additional connections, the incoming
connection will be handled in the same process and will exit when the
client disconnects.  Use this for easier debugging.

## Environment variables

- `GLS_DONT_FILTER=1`: don't filter API extensions exposed on the
  server (by default we filter to only expose the subset supported by
  GLS).  This is meant to help finding out which optional extensions
  client apps would use, through subsequent `GetGlesProcAddress` which
  will then be logged on the client as "available on server but not
  supported".

# Launch client

- Override system libraries (tune with your own build dir)
  ```
  export LD_LIBRARY_PATH=$(realpath build/gl_client):$LD_LIBRARY_PATH
  ```
- Use test programs: run `eglinfo`, `es2_info`, `es2gears`, `es2tri`, `glmark2`, etc.
- Run a real program using OpenGL ES

## Environment variables

### `GLS_DEBUG` -- log executing/flushing command to the console

Disabled by default.  Valid values:
- 0: Disable command logging (default).
- 1: Enable command logging.

### `GLS_SERVER_ADDR` -- select (transport-specific) server address
See below.

### `GLS_TRANSPORT` -- select transport used to connect to server
See below.

# Available transports

Transport is selected on server side by `-t` option, and on client
side by `GLS_SERVER_ADDR` environment variable.

Default value: `tcp`.

Allowed values:

- `tcp`: use TCPv4.  Superserver waits for new clients, and forks a
  dedicated child server to handle the connection to the client.
  - `GLS_SERVER_ADDR` can be `<IP>` or `<IP>:<port>`
  - when `<port>` is omited, defaults to `18145`
  - default value: `127.0.0.1`

  `0.0.0.0` will listen for connections over the network, but keep
  in mind there is absolutely no way today to authorize individual
  connections

- `stdio`: use stdin/stdout for a direct connection (on server side
  this implies not using a superserver).  Typically spawned by an
  external process setting up those file descriptors (eg. by QubesOS
  qrexec daemon acting as superserver).  Does not use
  `GLS_SERVER_ADDR`.

  Can be used on client side using `qrexec-client-vm` to setup the
  file descriptors, but this has several restrictions:

  - the use of stdin/stdout implies that the client app _must not_ use
    them.  Eg. using glmark2 with such a setup requires patching it to
    use `cerr` instead of `cout` for its reporting.
  - `qrexec-client-vm` wants an absolute path to executable

  ```
   $ GLS_TRANSPORT=stdio LD_LIBRARY_PATH=$PWD/build/gl_client \
     qrexec-client-vm dom0 qubes.GLS /usr/bin/es2gears
  ```

  Can also be used as a local debug/comparison tool, using named pipes
  with a setup such as:

  ```
  $ mkfifo tosrv
  $ mkfifo fromsrv
  $ < tosrv > fromsrv ./build/gl_server/gl_server -t stdio
  $ > tosrv < fromsrv GLS_TRANSPORT=stdio LD_LIBRARY_PATH=$PWD/build/gl_client es2gears
  ```

  Beware that the redirections through the named pipes are setup by
  the shell prior to launching the program, and under Linux the
  openning them is blocking until the other end is opened too, so:
  - the redirections must be set up in the same order on both command
    lines
  - the server will not start, so cannot notify of progress or syntax
    error, before the client is launched

- `qrexecpipe`: for client-side only, communicates with the Qubes
  qrexec system using pipes to `qrexec-client-vm`
  - `GLS_SERVER_ADDR` can be `<domain>` or `<domain>:<service>`
  - when `<service>` is omited, defaults to `qubes.GLS`
  - default value: `dom0`.  Useful alternate value is `sys-gui-gpu`.


# QubesOS test setup

Be sure you understand the security implications before trying this:
this software is experimental, has known security issues, and you're
going to run it in a sensible area of your QubesOS system.

Also be aware, even if you trust the `gl_server` code, that the
programs you run in your (untrusted) domain will cause execution of
code in your `dom0` or `sys-gui-gpu` (GPU driver stack), that is
usually not exercised by QubesOS, and which as such does not receive
(in the case of `dom0`) the security updates that a graphics stack
should receive.  This **it is strongly discouraged** to run this in
`dom0`, the more so using proprietary driver stack, the more so using
untrusted programs (eg. closed-source apps).


## in GPU domain (`dom0` or `sys-gui-gpu`)

### transfer `gl_server` from your build qube to GPU domain

Transfer the executable, eg. with:

```
$ qvm-run -p perso-dev 'tar -zc gl_server' | tar -xz
```

Then install it, eg. in `/usr/local/bin/`.

### declare a Qubes RPC service

As root, create a RPC service script, eg. from provided
[`qubes.GLS`](../qubes-os/qubes.GLS), as `/etc/qubes-rpc/qubes.GLS` (this
is the default service used by `qrexecpipe` transport, but you may
adjust at will) and make it executable executable.  You may need to
adjust the path to `gl_server`.

After sending a desktop notification for safety, it will launch a
`gl_server` to handle your GLES2 app's connection.

## in client qube

Launch your apps as previously described, using the `qrexecpipe`
transport.  E.g.:

```
$ GLS_TRANSPORT=qrexecpipe GLS_SERVER_ADDR=sys-gui-gpu LD_LIBRARY_PATH=$PWD/build/gl_client /usr/bin/es2gears
```


# C preprocessor contionnals

This will likely only be useful to developers.

## Server
### GL_DEBUG
- Enable debug log write to stderr.
- Disabled by default.

## Client
### USE_X11
- legacy conditional, should always be set
- Enable on client side for support for X11 apps
- Enable on server side to render in X11 window
- Enabled by default
