# Setup streaming environment

No installation is required to test the software.

## Server setup

### Linux

```
make -C gl_server
```

### Android (code probably not uptodate, may not build)
- Build Server app using Android Studio with NDK installed.
- Install server APK. 

## Client setup

```
make -C gl_client
```

# Launch server

```
gl_server
```

Optionally, use `-s` and `-c` flags to specify non-default server and
client `ip:port` (defaults `127.0.0.1:18145` and `127.0.0.1:18146`
respectively).

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
- Enable support for X11 apps.
- Enabled by default.
- Sample side doesn't use X11 so not enabled.

### GLS_USE_CLTSIZE
- Use client X11 window size as surface size (instead of querying server
  and getting the fixed window size we currently have)
- Only meaningful if USE_X11 is enabled.
- Enabled by default.
- May be useful to disable scaling of rendering to server window size.

