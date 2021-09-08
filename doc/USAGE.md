# Setup streaming environment

No installation is required to test the software.

Use `make` in either one directory `gl_server` or `gl_client`, or from
toplevel to build both at the same time.  If you need to tweek the
code, beware that source dependency tracking is not available yet.

# Launch server

```
gl_server
```

Optionally, use `-s` flags to specify non-default server `ip:port`
(default `127.0.0.1:18145`).  The port can be omitted, and defaults to
`18145`.  Use `-s 0.0.0.0` to listen for connections over the network,
but keep in mind there is absolutely no way today to authorize
individual connections.

## Environment variables

- `GLS_DONT_FILTER=1`: don't filter API extensions exposed on the
  server (by default we filter to only expose the subset supported by
  GLS).  This is meant to help finding out which optional extensions
  client apps would use, through subsequent `GetGlesProcAddress` which
  will then be logged on the client as "available on server but not
  supported".

# Launch client

- Override system libraries
  ```
  export LD_LIBRARY_PATH=/path/to/gl-streaming/gl_client:$LD_LIBRARY_PATH
  ```
- Use test programs: run `eglinfo`, `es2_info`, `es2gears`, `es2tri`, `glmark2`, etc.
- Run a real program using OpenGL ES

## Environment variables

### `GLS_DEBUG` -- log executing/flushing command to the console

Disabled by default.  Valid values:
- 0: Disable command logging (default).
- 1: Enable command logging.

### `GLS_SERVER_ADDR` -- set server IP address and port

Default value: `127.0.0.1:18145`.


# C preprocessor contionnals

This will likely only be useful to developers.

## Server
### GL_DEBUG
- Enable debug log write to stderr.
- Disabled by default.

## Client
### USE_X11
- Enable on client side for support for X11 apps
- Enable on server side to render in X11 window
- Enabled by default
