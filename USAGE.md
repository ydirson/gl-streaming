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

# Copy GL4ES into (optional)
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
- Run an program requires OpenGL or OpenGL ES (many: Xvfb, x-window-manager, etc...).

# Variables on compile
## Sever
### • GL_DEBUG
- Enable debug log write to `/sdard/mthr_log.txt`.
- Disabled by default.

## Client
### USE_X11
- Enable support for X11 apps.
- Enabled by default.
- Sample side doesn't use X11 so not enabled.

### • GLS_USE_SRVSIZE
- Use server window size instead of X Window size.
- Disabled by default.
- This is enabled if USE_X11 is not enabled.
- May useful for fixing draw size bugs.

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

