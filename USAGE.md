## Setup streaming environment
### Server setup
- Build Server app using Android Studio with NDK installed.
- Install server APK.

### Client setup
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

## Launch streaming!
### Launch server
- Do as title: Launch server.
- Enter server port and client address, or leave empty for default values.

### Launch client
- Test: run `es2_info, es2gears, es2tri, glxinfo, glxgears, glxdemo or glxheads`
- Run an OpenGL Native Program or Java Program.

## Variables on compile
- (Server) `GL_DEBUG`: enable debug log write to `/sdard/mthr_log.txt`. Disabled by default.

## Environment variable on launch
- (Client) `GLS_DEBUG`: print executing/flushing command to the console. Disabled by default.
- (Client) `GLS_SERVER_IP`: set server IP address.
