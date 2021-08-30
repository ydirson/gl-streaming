# TODO list for proper Linux and Qubes support

- [ ] work on native linux
  - [x] disable server GLES extensions, symbols not exported by standard libs
  - [x] default window size
  - [x] proper line endings in logs
  - [x] proper server logging, not hardcoded /sdcard/
  - mesa-demos coverage
    - EGL/GLES2
      - [x] eglinfo, es2_info support
      - [x] es2tri support
      - [x] es2gears support
    - EGL/OpenGL
      - [ ] egltri support (needs glMatrixMode)
      - [ ] eglgears support (needs glLightfv)
      - [ ] xeglgears support (needs eglCreateImageKHR)
      - xeglthreads support
        - [ ] `xeglthreads -n 1` (needs glMatrixMode)
        - [ ] `xeglthreads` (needs multiple contexts)
  - glmark2 coverage (could use `EGL_EXT_platform_base` for `eglGetPlatformDisplayEXT`)
    - build
      - [x] use-vbo=false
      - [x] use-vbo=true
    - texture
      - [x] texture-filter=nearest
      - [x] texture-filter=linear
      - [x] texture-filter=mipmap
    - shading
      - [x] shading=gouraud
      - [x] shading=blinn-phong-inf
      - [x] shading=phong
      - [x] shading=cel
    - bump
      - [x] bump-render=high-poly
      - [x] bump-render=normals
      - [x] bump-render=height
    - effect2d
      - [x] kernel=0,1,0;1,-4,1;0,1,0;
      - [x] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1;
    - pulsar
      - [x] light=false:quads=5:texture=false
    - desktop
      - [x] blur-radius=5:effect=blur:passes=1:separable=true:windows=4
      - [x] effect=shadow:windows=4
    - buffer
      - [ ] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map (needs `GL_OES_mapbuffer`)
      - [x] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata
      - [ ] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map (needs `GL_OES_mapbuffer`)
    - ideas
      - [.] speed=duration (no error, but missing some objects)
    - [x] jellyfish
    - [x] terrain
    - [x] shadow
    - [x] refract
    - conditionals
      - [x] fragment-steps=0:vertex-steps=0
      - [x] fragment-steps=5:vertex-steps=0
      - [x] fragment-steps=0:vertex-steps=5
    - function
      - [x] fragment-complexity=low:fragment-steps=5
      - [x] fragment-complexity=medium:fragment-steps=5
    - loop
      - [x] fragment-loop=false:fragment-steps=5:vertex-steps=5
      - [x] fragment-steps=5:fragment-uniform=false:vertex-steps=5
      - [x] fragment-steps=5:fragment-uniform=true:vertex-steps=5
  - SDL2 programs with a GLES2-only libSDL
    - [x] blobby
    - [x] chocolate-doom
    - [.] gigalomania: seems to work, but limited by use of absolute mouse coordinates
          and lack of window resizing
    - [.] planetblupi: seems to work, but limited by use of absolute mouse coordinates
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
    - [ ] replace 2-buffers design by a buffer pool instead of memcpy'ing between
          network layer and queue layer
- fixes
  - [x] non-implemented functions (eg. glIsEnabled) cause client to segfault because
        symbol is NULL, core functions must all be provided
  - [.] stop passing any kind of pointer over the wire (huge security issue, although
        we could mitigate that aspect by tracking valid pointer values, but also
        problematic for 64bit platforms, when casting pointers into 32bit integers...)
  - [x] find out which EGL and GLES standard version are precisely covered today
  - [x] fix texture support
  - [x] funky issues around glCullFace: GL_BACK rejected as GL_INVALID_ENUM
  - [x] server leaks non-freed resources
  - [ ] let eglQuerySurface do all size queries except for the window one, don't stub
        all of them
  - [.] filter eglQueryString output for EGL_EXTENSIONS and EGL_CLIENT_APIS
  - [ ] ensure non-modification of output params on error (API compliance)
  - [ ] proper *GetError semantics
  - [ ] don't use GLint in protocol? (specified as platform-dependant, even though
        Mesa uses 32bit even on 64bit platforms)
  - [x] apitrace loops, requesting GLES3 items with glGetIntegerv
- improve coverage
  - [ ] full EGL core
    - [x] non-stub config handling
    - [ ] non-default EGLDisplay handling
    - [x] non-stub context management
  - [ ] EGL extension support
  - [ ] proper GL_OES_mapbuffer implementation
  - [ ] separate EGL and GLSclient libs out of libGLES2
  - [ ] full GLES2 core
  - [ ] GLES2 extension support
  - [ ] GLES3 ?
  - [ ] desktop GL with EGL ?
  - [ ] GLX ?
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
      it cannot load its data files (ie. not run from samples dir
