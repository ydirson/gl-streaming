# OpenGL streaming

This software provides an implementation of the EGL and GLES2 APIs,
which forwards an application's (client-side) graphics calls to a
remote (server-side) EGL/GLES2 implementation, which typically renders
using a GPU.  Any display operations are done by the server, on its
own screen.

It is expected to get extended in the future to support more graphics
APIs (eg. desktop OpenGL, GLX, Vulkan), as very few apps work with
GLES2.

The motivation for this project is to provide GPU acceleration to
virtual machines, with rendering on their host, especially to be
usable on [QubesOS](https://www.qubes-os.org/) to securely provide
decent GPU acceleration to Qubes containers.

The GPU server should be able to run on most UNIX-like systems.  The
client library will be adapted to work on various plaforms suitable to
be used in a Qube.  Either way, today it is only tested on GNU/Linux
platforms.


# Current status

This is still a work in progress.  See [current software support
status](doc/software-support.md) for detailed per-application
compatibility information.  While the core functionnality (support for
forwarding GL calls) is there, the problem with window handling
(described below) is critical enough to postpone work on security
improvements.  In short: consider this version as a PoC, test in
controlled environment, and **don't deploy it**.

Notable information:

* A number of APIs are still stubbed, and some are known to have bugs,
  the holes are plugged progressively to add support for more client
  apps.  Very few extensions are already supported.

* APIs for EGL 1.4 are present, though some 1.1 and 1.2 APIs are not
  even stubbed.  Version 1.0 is advertised, even though it is not
  fully implemented yet.

* Only EGL/X11 is properly supported for now

* When not using QubesOS integration,
  the client still creates a window on its own, which is used for
  interaction with the app (input events, window resize, etc).  When
  running locally (ie. GPU server and app on the same display) it gets
  quite awkward, as both windows must share the same screen.  However,
  resizing the local window will allow some apps to be used, at the
  cost of using a smaller window so they both fit into the screen, as
  long as the "input window" keeps the focus.  This trick can be
  extended to apps that don't use the mouse at all.

  Both application classes can be easier to use remotely.

  However, applications making real use of the mouse and not drawing
  their own mouse cursor cannot realistically be used yet.

* Application relying on SDL2 to get an OpenGL context need an
  additional trick, as the SDL2 library shipped in most Linux
  distributions will prefer the use of Desktop OpenGL.  SDL2 ought to
  obey `SDL_RENDER_DRIVER=opengles2` in the environment but apparently
  doesn't, so for now we have to build our own libsdl2 without desktop
  GL support.

* Unfortunately very few 3D applications for the Desktop use GLES2.
  Please open a ticket if have one at hand.

* There are security concerns, notably:
  * usage of pointer types in the protocol must be cured
  * when using the TCP transport (which mostly exists for testing
    purposes today), any app can connect to the server (somewhat
    mitigated by listening only on `localhost` by default, but you'd
    better trust other users of a system before launching `gl_server`
    on it)

* Usage of pointer types in the protocol also implies that only 64bit
  builds can work now.  We should not only be able to build 32bit
  versions too, but to have them interoperate.

* [A rather unsorted TODO list](doc/TODO.md) has a summary of things
  to be done, as well as those already done: it's important to see
  that despite the large amount of work to be done, things are really
  progressing.

* Essentially tested both client and server sides on GNU/Linux.

* Apps which do not control vsync using `eglSwapInterval()`
  (eg. `es2gears`), which need to set `vblank_mode=0` to force disable
  vsync rendering, need to do so _on the `gl_server`_ process.


# How to use

See [usage instructions](doc/USAGE.md), and especially a [QubesOS test
setup](doc/USAGE.md#qubesos-test-setup).


# High-level roadmap

This is my own personal roadmap.  Contributors welcome, to help with
those items or add their own in the picture.

* [ ] bring EGL/GLES2 support to a satisfying state
  * [x] add extension support
  * [ ] solve structural security issues
    * [x] use separate processes to isolate clients' contexts on server
    * [ ] remove pointers from protocol
  * [x] implement a satisfying window/input experience
    * [x] on-demand window, properly sized
    * [x] no extra client-side window (on QubesOS)
  * [ ] bring to decent performance level
* [ ] add Vulkan support


# Performance and conformance information

Very few [preliminary benchmarks](benchmarks/) were run.


# Other sources of information

* [Home page](https://github.com/ydirson/gl-streaming)
* [Usage informations](doc/USAGE.md), though still a bit terse
* [An overview of inner workings](doc/INTERNALS.md)
* [Historical information](doc/README.shodruky-rhyammer.md) from
  author of the original project, including a nice visual explanation


# Reporting bugs

If you find a bug, wish to report success/failure with a given app, or
submit any other relevant item,
see the [project issues on Github](https://github.com/ydirson/gl-streaming/issues).


# Thanks

This work builds on previous efforts:

[Shodruky Rhyammer](https://github.com/shodruky-rhyammer/gl-streaming)
wrote the original PoC, client-side requiring patching apps to use
streaming, and server-side rendering on a Raspberry Pi GPU.  Cites
meaningful help from OpenGL forum (notably from user The Little Body).

[ArtDev](https://github.com/artdeell/gl-streaming) implemented
server-side rendering on Android devices (abandonning support for
proprietary RPi-specific APIs), and started support for a rendering on
a Linux-based server.

[Mittorn](https://github.com/mittorn/gl-streaming.git) implemented
emulation of client arrays with VBO's (later apparently reused by Duy
Tran Khanh forgetting to credit the author of original code).

[Duy Tran
Khanh](https://github.com/khanhduytran0/android-gl-streaming) greatly
enhanced API covererage, turned the client side into a real EGL/GLES2
shared lib allowing existing apps to run unmodified.
