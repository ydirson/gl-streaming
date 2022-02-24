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

## in GPU domain (`dom0` or `sys-gui-gpu`)

### declare a Qubes RPC service

As root, create a script, eg. `/etc/qubes-rpc/xqubes.GLS`, and make it
executable executable.  After sending a desktop notification for
safety, it will forward the connection to a `gl_server` running on its
default port:

```
#!/bin/sh
notify-send "dom0: GLS access from $QREXEC_REMOTE_DOMAIN"
socat - TCP:localhost:18145
```

### transfer `gl_server` from your build qube to GPU domain

```
$ qvm-run -p perso-dev 'tar -zc gl_server' | tar -xz
```

### launch `gl_server`

Since this software is experimental, you want to launch it by hand
(not as a permanent service), and not as root.

```
$ ./gl_server
```

## in client qube

Just setup a TCP-to-qrexec bridge as follows, and launch your apps as
previously described.

```
$ socat "TCP-LISTEN:18145,fork" "EXEC:qrexec-client-vm dom0 xqubes.GLS"
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
