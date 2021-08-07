# OpenGL streaming

This software provides an implementation of the EGL and GLES2 APIs,
which forwards the calls to a remote EGL/GLES2 implementation, which
typically renders using a GPU.  It is expected to be extended to
support more graphics APIs (eg. desktop OpenGL, GLX, Vulkan).

The motivation for this project is to provide GPU acceleration to
virtual machines, with rendering on their host, especially to be
usable on [QubesOS](https://www.qubes-os.org/) to securely provide
decent GPU acceleration to Qubes containers.


# Current status

This is still a work in progress, notably:

* only 32bit builds can work at all (addressing this is top priority).

* despite all the work previously done, only very simple applications
  work (eg. es2gears, some glmark2 benchmarks).  A number of APIs are
  present but only as stubs, some are known to have bugs -- some can
  cause the client or the server to crash.

* there are security concerns about the usage of pointer types in the
  protocol.

* no protocol extension is supported yet, until proper generic
  extension support gets implemented.

* [a rather unsorted TODO list](TODO.md) has a summary of things to be
  done, as well as those already done: it's important to see that
  despite the large amount of work to be done, things are really
  progressing.

* Android support does not get tested or updated, patches welcomed.


# Performance and conformance information

... will be published once we reach a reasonable coverage of glmark2
tests.  It is hoped that performance will appear to be quite
reasonable.


# Other sources of information

* [Usage informations](USAGE.md), a bit terse and somewhat targetting
  Android builds
* [An overview of inner workings](INTERNALS.md)
* [Historical information](README.shodruky-rhyammer.md) from author of
  the original project, including a nice visaul explanation


# Thanks

This work builds on previous efforts:

[Shodruky Rhyammer](https://github.com/shodruky-rhyammer/gl-streaming)
wrote the original PoC, client-side requiring patching apps to use
streaming, and server-side rendering on a Raspberry Pi GPU.

[ArtDev](https://github.com/artdeell/gl-streaming) implemented
server-side rendering on Android devices (abandonning support for
proprietary RPi-specific APIs), and started support for a rendering on
a Linux-based server.

[Duy Tran
Khanh](https://github.com/khanhduytran0/android-gl-streaming) greatly
enhanced API covererage, turned the client side into a real EGL/GLES2
shared lib allowing existing apps to run unmodified.
