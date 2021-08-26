/*
Copyright (c) 2013, Shodruky Rhyammer
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "glcontrol.h"
#include "fastlog.h"

#include <GLES2/gl2.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int glsurfaceview_width;
int glsurfaceview_height;

int   var_server_port;
int   var_client_port;
const char* var_client_addr;
/*
const char* var_path_shadercache;
const char* var_file_fragment_shader;
const char* var_file_vertex_shader;
*/

EGLConfig config;

static void make_egl_base(graphics_context_t* gc);

//#define DEBUG
#define CASE_STRING( value ) case value: return #value;
const char* eglGetErrorString(EGLint err)
{
  switch (err) {
    CASE_STRING( EGL_SUCCESS             )
    CASE_STRING( EGL_NOT_INITIALIZED     )
    CASE_STRING( EGL_BAD_ACCESS          )
    CASE_STRING( EGL_BAD_ALLOC           )
    CASE_STRING( EGL_BAD_ATTRIBUTE       )
    CASE_STRING( EGL_BAD_CONTEXT         )
    CASE_STRING( EGL_BAD_CONFIG          )
    CASE_STRING( EGL_BAD_CURRENT_SURFACE )
    CASE_STRING( EGL_BAD_DISPLAY         )
    CASE_STRING( EGL_BAD_SURFACE         )
    CASE_STRING( EGL_BAD_MATCH           )
    CASE_STRING( EGL_BAD_PARAMETER       )
    CASE_STRING( EGL_BAD_NATIVE_PIXMAP   )
    CASE_STRING( EGL_BAD_NATIVE_WINDOW   )
    CASE_STRING( EGL_CONTEXT_LOST        )

  default: return ("UNKNOWN EGL ERROR");
  }
}
const char* glGetErrorString(GLenum err)
{
  switch (err) {
    CASE_STRING(GL_NO_ERROR)
    CASE_STRING(GL_INVALID_ENUM)
    CASE_STRING(GL_INVALID_VALUE)
    CASE_STRING(GL_INVALID_OPERATION)
    CASE_STRING(GL_OUT_OF_MEMORY)
  case 0x8031: /* not core */ return "GL_TABLE_TOO_LARGE_EXT";
  case 0x8065: /* not core */ return "GL_TEXTURE_TOO_LARGE_EXT";
    CASE_STRING(GL_INVALID_FRAMEBUFFER_OPERATION)

  default: return ("UNKNOWN GL ERROR");
  }
}
#undef CASE_STRING

void base_check_egl_err(const char* funcname)
{
  int error = eglGetError();
  if (error != EGL_SUCCESS) {
    LOGD("GLS ERROR: eglGetError(%s) return error %s\n", funcname, eglGetErrorString(error));
  }
#ifdef DEBUG
  assert(error == 0)
#endif // DEBUG
}
void base_check_gl_err(const char* funcname)
{
  int error = glGetError();
  if (error != GL_NO_ERROR) {
    LOGD("GLS ERROR: glGetError(%s) return error %s\n", funcname, glGetErrorString(error));
  }
#ifdef DEBUG
  assert(error == 0)
#endif // DEBUG
}

void init_egl(graphics_context_t* gc)
{
#ifdef USE_X11
  gc->x.display = XOpenDisplay(NULL);
  if (!gc->x.display) {
    fprintf(stderr, "GLS ERROR: couldn't open display %s\n", getenv("DISPLAY"));
    exit(EXIT_FAILURE);
    return;
  }
  gc->display = eglGetDisplay(gc->x.display);
#else
  gc->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
  assert(gc->display != EGL_NO_DISPLAY);
  check_gl_err(eglGetDisplay);

  gc->screen_width = glsurfaceview_width; // (gc->d_rect.right - gc->d_rect.left);
  gc->screen_height = glsurfaceview_height; // (gc->d_rect.bottom - gc->d_rect.top);
  // gc->d_window = glsurfaceview_window;
  // assert (glsurfaceview_window != NULL);

#ifdef USE_X11
  make_egl_base(gc);
#endif
}


void release_egl(graphics_context_t* gc)
{
  eglMakeCurrent(gc->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(gc->display, gc->context);
  eglTerminate(gc->display);
  eglReleaseThread();
}


/**************************************************************************
 *
 * Copyright 2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 *
 **************************************************************************/

/*
 * Create an RGB, double-buffered X window.
 * Return the window and context handles.
 */

#ifdef USE_X11
static void make_egl_base(graphics_context_t* gc)
{
  if (!eglInitialize(gc->display, NULL, NULL)) {
    fprintf(stderr, "Error: eglInitialize() failed\n");
    exit(1);
  }
}

void gls_create_x11_window(graphics_context_t* gc, const char* name, int x, int y, int width, int height)
{
  XSetWindowAttributes attr;
  unsigned long mask;
  Window root;
  XVisualInfo* visInfo, visTemplate;
  int num_visuals;
  int xScreenId = DefaultScreen(gc->x.display);
  root = RootWindow( gc->x.display, xScreenId );

  /* The X window visual must match the EGL config */
  visTemplate.visualid = XVisualIDFromVisual(XDefaultVisual(gc->x.display, xScreenId));
  visInfo = XGetVisualInfo(gc->x.display, VisualIDMask, &visTemplate, &num_visuals);
  if (!visInfo) {
    fprintf(stderr, "GLS ERROR: couldn't get X visual\n");
    exit(1);
  }

  /* window attributes */
  attr.background_pixel = 0;
  attr.border_pixel = 0;
  attr.colormap = XCreateColormap( gc->x.display, root, visInfo->visual, AllocNone);
  attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
  mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  gc->x.window = XCreateWindow( gc->x.display, root, 0, 0, width, height,
                                0, visInfo->depth, InputOutput,
                                visInfo->visual, mask, &attr );

  /* set hints and properties */
  {
    XSizeHints sizehints;
    sizehints.x = x;
    sizehints.y = y;
    sizehints.width  = width;
    sizehints.height = height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(gc->x.display, gc->x.window, &sizehints);
    XSetStandardProperties(gc->x.display, gc->x.window, name, name,
                           None, (char**)NULL, 0, &sizehints);
  }

  XFree(visInfo);
  XMapWindow(gc->x.display, gc->x.window);
}
#else
#error "GLS FIXME: only supporting server on X11 platform"
#endif
