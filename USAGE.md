# Setup streaming environment
## Server setup
- Build Server app using Android Studio with NDK installed.
- Install server APK. 

## Client setup
```
cd /path/to/android-gl-streaming/gl_client

# Make the client
make

# Install gl-streaming client
make install

# Copy GL4ES into
cp /path/to/gl4es/lib/libGL.so.1 /usr/local/lib/gl-streaming

# Override system libGLESv2.so.2, libEGL.so.1 and libGL.so.1
export LD_LIBRARY_PATH=/usr/local/lib/gl-streaming:$LD_LIBRARY_PATH
```

# Launch streaming!
## Launch server
- Do as title: Launch server.
- Enter server port and client address, or leave empty for default values.

## Launch client
- Test: run `es2_info, es2gears, es2tri, glxinfo, glxgears, glxdemo or glxheads`
- Run an OpenGL Native Program or Java Program.

# Variables on compile
## Sever
- `GL_DEBUG`: Enable debug log write to `/sdard/mthr_log.txt`. Disabled by default.

## Client
### `GLS_USE_SRVSIZE`
- Use server window size insread of X Window size.
- Enabled by default.
- May be removed later.
### `GLS_EMULATE_VBO`
- Try to emulate client arrays with VBO.
- Enabled by default.
- This is experimental, disable may make `es2tri` or some programs show blank screen.

# Environment variable on launch
## Server
- Nothing yet.

## Client
### • GLS_DEBUG
- Log executing/flushing command to the console. Disabled by default.
- 0: Disable command logging (default).
- 1: Enable command logging.

### • GLS_SERVER_ADDR
- Set server IP address and port.
- Default value: `127.0.0.1:18145`.
