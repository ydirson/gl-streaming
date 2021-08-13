# TODO list for proper Linux and Qubes support

- [ ] work on native linux
  - [x] disable server GLES extensions, symbols not exported by standard libs
  - [x] default window size
  - [x] proper line endings in logs
  - [ ] proper server logging, not hardcoded /sdcard/
  - [ ] enough EGL/GLES2 coverage to use mesa-demos examples
    - [x] eglinfo, es2_info support
    - [ ] egltri support
    - [x] es2tri support
    - [ ] eglgears support
    - [x] es2gears support
    - [ ] acceptable glmark2 coverage
- [x] readable indentation
- [ ] work on native 32bit linux (now broken by hacking in 64bit support)
- [x] work on native 64bit platforms
  - [ ] as a first step include word-size in protocol if needed
- improve code quality
  - [x] take compiler warnings into account
  - [ ] more readable idioms
  - [ ] audit string/data-size usages
  - [ ] replace custom window creation with a standard portable library
- improve performance
  - [ ] make the fifo poll()able instead of using usleep-based active polling
  - [ ] zero-copy when possible
    - [ ] replace 2-buffers design by a buffer pool instead of memcpy'ing between
          network layer and queue layer
  - [ ] reduce malloc/free
- fixes
  - [ ] stop passing any kind of pointer over the wire (huge security issue, although
        we could mitigate that aspect by tracking valid pointer values, but also
        problematic for 64bit platforms, when casting pointers into 32bit integers...)
  - [x] find out which EGL and GLES standard version are precisely covered today
  - [ ] let eglQuerySurface do all size queries except for the window one, don't stub
        all of them
  - [.] filter eglQueryString output for EGL_EXTENSIONS and EGL_CLIENT_APIS
  - [ ] non-stub eglInitialize
  - [ ] ensure non-modification of output params on error (API compliance)
  - [ ] don't use GLint in protocol? (specified as platform-dependant, even though
        Mesa uses 32bit even on 64bit platforms)
  - [ ] handle window size, create windows on demand, not just a fixed one
- improve coverage
  - [ ] full EGL core
    - [x] non-stub config handling
    - [ ] non-default EGLDisplay handling
    - [ ] non-stub context management
  - [ ] EGL extension support
    - [ ] proper GL_OES_mapbuffer implementation
  - [ ] separate EGL and GLSclient libs out of libGLES2
  - [ ] full GLES2 core
  - [ ] GLES2 extension support
  - [ ] GLES3 ?
  - [ ] GLX / desktop GL ?
  - [ ] Vulkan !
- [ ] update README
- [ ] consistent coding style

## medium priority

- [ ] regression testing
- performance
  - [ ] bench
  - [ ] avoid repeated eglGetCurrentDisplay() calls in serveregl
- [ ] consider migrating this inside Mesa and upstreaming

## low priority

- [ ] sample2 causes server to fail with "Error: Command Flush -10256991" when
      it cannot load its data files (ie. not run from samples dir
