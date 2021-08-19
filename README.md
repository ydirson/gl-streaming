# OpenGL streaming

This software provides an implementation of the EGL and GLES2 APIs,
which forwards an application's (client-side) graphics calls to a
remote (server-side) EGL/GLES2 implementation, which typically renders
using a GPU.  Any display operations are done by the server, on its
own screen.

It is expected to get extended in the future to support more graphics
APIs (eg. desktop OpenGL, GLX, Vulkan).

The motivation for this project is to provide GPU acceleration to
virtual machines, with rendering on their host, especially to be
usable on [QubesOS](https://www.qubes-os.org/) to securely provide
decent GPU acceleration to Qubes containers.

The GPU server should be able to run on most UNIX-like systems.  The
client library will be adapted to work on various plaforms suitable to
be used in a Qube.  Either way, today it is only tested on GNU/Linux
platforms.


# Current status

This is still a work in progress, notably:

* despite all the work previously done, only very simple applications
  work (eg. es2gears, es2tri, some glmark2 benchmarks).

  A number of APIs are present but only as stubs, some are known to
  have bugs.

  APIs for EGL 1.4 are present, though some 1.1 and 1.2 APIs are not
  even stubbed.

* the server creates a fixed-size window at startup, used for all
  client rendering.  The client still creates a window on its own,
  which is used for interaction with the app (input events, window
  resize, etc), which is quite awkward.

* there are security concerns about the usage of pointer types in the
  protocol.

* usage of pointer types in the protocol also implies that only 64bit
  builds can work now.  We should not only be able to build 32bit
  versions too, but to have them interoperate.

* no protocol extension is supported yet, until proper generic
  extension support gets implemented.

* [a rather unsorted TODO list](TODO.md) has a summary of things to be
  done, as well as those already done: it's important to see that
  despite the large amount of work to be done, things are really
  progressing.

* Essentially tested both client and server sides on GNU/Linux.


# Performance and conformance information

... will be published once we reach a reasonable coverage of glmark2
tests.  It is hoped that performance will appear to be quite
reasonable.


# Other sources of information

* [Usage informations](USAGE.md), though still a bit terse
* [An overview of inner workings](INTERNALS.md)
* [Historical information](README.shodruky-rhyammer.md) from author of
  the original project, including a nice visual explanation


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
Tran Khanh without mentionning author of original code).

[Duy Tran
Khanh](https://github.com/khanhduytran0/android-gl-streaming) greatly
enhanced API covererage, turned the client side into a real EGL/GLES2
shared lib allowing existing apps to run unmodified.
