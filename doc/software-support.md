# Current software support status

| program | status | caveats |
| --- | --- | --- |
| [EGL-X11/ES2 programs mesa-demos](https://gitlab.freedesktop.org/mesa/demos) | OK | es2tri requires [removal of an abusive assertion](https://gitlab.freedesktop.org/mesa/demos/-/merge_requests/46) |
| [examples from the book "OpenGL ES 2.0 Programming Guide"](https://github.com/danginsburg/opengles-book-samples.git) | OK | - |
| [glmark2-es2](https://github.com/glmark2/glmark2/) | OK | version 2021.02 would segfault, they have a fix in `master` branch; tests depending on the `GL_OES_mapbuffer` extension cannot work yet|
| [Blobby Volley 2](http://sourceforge.net/projects/blobby/) | OK | requires DesktopGL-disabled SDL2; needs to place "input window" suitably |
| [Gigalomania](http://gigalomania.sourceforge.net) | OK | requires DesktopGL-disabled SDL2; needs to place "input window" suitably |
| [Weston](https://github.com/wayland-project/weston) | OK | needs to place "input window" suitably; to run wayland apps from the builtin terminal, the native EGL/GLES2 libs must be used -- remove the path to GLS libs from your `LD_LIBRARY_PATH` |
| [prBoom+](http://prboom-plus.sourceforge.net/) | OK | requires DesktopGL-disabled SDL2; needs to place "input window" suitably |
| [Chocolate Doom](http://chocolate-doom.org/) | OK | requires DesktopGL-disabled SDL2; needs to place "input window" suitably |
| [Planet Blupi](http://blupi.org) | so-so | requires DesktopGL-disabled SDL2; needs to place "input window" suitably; does not draw its own mouse cursor |
