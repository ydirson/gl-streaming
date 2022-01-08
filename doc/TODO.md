# TODO list for proper Linux and Qubes support

- [ ] work on native linux
  - [x] disable server GLES extensions, symbols not exported by standard libs
  - [x] default window size
  - [x] proper line endings in logs
  - [x] proper server logging, not hardcoded `/sdcard/`
  - [x] full mesa-demos EGL/GLES2 coverage
  - full glmark2 coverage
    - [ ] ideas (scene misses several objects)
    - [ ] buffer (needs `GL_OES_mapbuffer`)
  - SDL2 programs with a GLES2-only libSDL
    - [x] blobby
    - [x] chocolate-doom
    - [x] gigalomania
    - [ ] planetblupi: works, but limited by lack of in-game mouse cursor
  - [x] work on native 64bit platforms
- [x] readable indentation
- improve usability
  - [x] handle window size, create windows on demand, not just a fixed one
        (addresses: non-stub eglInitialize)
  - [ ] get rid of "local window"
  - [ ] work on native 32bit linux (now broken by hacking in 64bit support)
- improve code quality
  - [x] take compiler warnings into account
  - [x] more readable idioms
  - [ ] audit string/data-size usages
  - [ ] replace custom window creation with a standard portable library
  - [ ] stop encapsulating return messages in a SEND_DATA
  - [ ] make messages standalone, getting rid of SEND_DATA
- improve performance
  - [ ] make the fifo poll()able instead of using usleep-based active polling
  - [ ] zero-copy when possible
    - [ ] virtio-based communication between app domain and GPU domain
          (needed for `GL_OES_mapbuffer` and such)
    - [ ] replace 2-buffers design by a buffer pool instead of memcpy'ing between
          network layer and queue layer
- fixes
  - [x] non-implemented functions (eg. glIsEnabled) cause client to segfault because
        symbol is NULL, core functions must all be provided
  - [ ] stop passing any kind of pointer over the wire (huge security issue, although
        we could mitigate that aspect by tracking valid pointer values, but also
        problematic for 64bit platforms, when casting pointers into 32bit integers...)
  - [x] find out which EGL and GLES standard version are precisely covered today
  - [x] fix texture support
  - [x] funky issues around glCullFace: GL_BACK rejected as GL_INVALID_ENUM
  - [x] server leaks non-freed resources
  - [x] let eglQuerySurface do all size queries except for the window one, don't stub
        all of them
  - [ ] window surface creation always creates a new server window
        even (weston window resizing results in many server windows
        created)
  - [ ] `glmark2-es2-wayland` in weston segfaults
  - [.] filter eglQueryString output for EGL_EXTENSIONS and EGL_CLIENT_APIS
  - [ ] don't use GLint in protocol? (specified as platform-dependant, even though
        Mesa uses 32bit even on 64bit platforms)
- compliance fixes
  - [ ] ensure non-modification of output params on error
  - [ ] proper *GetError semantics
  - [x] apitrace loops, requesting GLES3 items with glGetIntegerv
- improve coverage
  - useful to weston
    - [ ] `GL_EXT_unpack_subimage`, needs special alignment handling
    - [ ] `EGL_EXT_buffer_age`
    - [ ] `EGL_EXT_swap_buffers_with_damage`, `EGL_KHR_swap_buffers_with_damage`
    - [ ] `EGL_ANDROID_native_fence_sync`
    - [ ] `EGL_KHR_surfaceless_context`
  - [ ] full EGL core
    - [x] non-stub config handling
    - [x] non-default EGLDisplay handling
    - [x] non-stub context management
  - [x] EGL extension support
  - [ ] proper `GL_OES_mapbuffer` implementation
  - [ ] separate EGL and GLSclient libs out of libGLES2
  - [ ] full GLES2 core
  - [x] GLES2 extension support
  - [ ] GLES3 ?
  - [ ] desktop GL with EGL ?
  - [ ] GLX ?
  - [ ] Wayland support ?
  - [ ] Vulkan !
- [x] update README
- [x] consistent coding style

## medium priority

- [ ] regression testing
- performance
  - [ ] bench
  - [x] avoid repeated eglGetCurrentDisplay() calls in serveregl
- [ ] consider migrating this inside Mesa and upstreaming

## low priority

- [ ] sample2 causes server to fail with "Error: Command Flush -10256991" when
      it cannot load its data files (ie. not run from samples dir)
