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
#include <X11/Xatom.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    LOGD("eglGetError(%s) return error %s\n", funcname, eglGetErrorString(error));
  }
#ifdef DEBUG
  assert(error == 0)
#endif // DEBUG
}
void base_check_gl_err(const char* funcname)
{
  int error = glGetError();
  if (error != GL_NO_ERROR) {
    LOGD("glGetError(%s) return error %s\n", funcname, glGetErrorString(error));
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
    LOGE("couldn't open display %s\n", getenv("DISPLAY"));
    exit(EXIT_FAILURE);
  }
  gc->x.fd = ConnectionNumber(gc->x.display);
  gc->x.property_atom.qubes_vmname = XInternAtom(gc->x.display, "_QUBES_VMNAME", False);
  gc->x.property_atom.qubes_vmwinid = XInternAtom(gc->x.display, "_QUBES_VMWINDOWID", False);

  gc->x.remote2local = list_new();
  if (!gc->x.remote2local) {
    LOGE("couldn't allocate window list\n");
    exit(EXIT_FAILURE);
  }
#endif
}


void release_egl(graphics_context_t* gc)
{
  (void)gc;
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
Window gls_create_x11_window(graphics_context_t* gc, const char* name, int x, int y, int width, int height)
{
  Window window;
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
    LOGE("couldn't get X visual\n");
    exit(1);
  }

  /* window attributes */
  attr.background_pixel = 0;
  attr.border_pixel = 0;
  attr.colormap = XCreateColormap( gc->x.display, root, visInfo->visual, AllocNone);
  attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
  mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  window = XCreateWindow(gc->x.display, root, 0, 0, width, height,
                         0, visInfo->depth, InputOutput,
                         visInfo->visual, mask, &attr);

  /* set hints and properties */
  {
    XSizeHints sizehints;
    sizehints.x = x;
    sizehints.y = y;
    sizehints.width  = width;
    sizehints.height = height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(gc->x.display, window, &sizehints);
    XSetStandardProperties(gc->x.display, window, name, name,
                           None, (char**)NULL, 0, &sizehints);
  }

  XFree(visInfo);
  XMapWindow(gc->x.display, window);

  return window;
}

Window gls_local_x11_window(graphics_context_t* gc, Window vmwindow)
{
  struct genlist* l = list_lookup(gc->x.remote2local, vmwindow);
  if (!l)
    return None;
  return (Window)(l->data);
}

static unsigned char* glse_vmname(graphics_context_t* gc, Window win)
{
  unsigned char* vmname;
  int format;
  Atom type_return;
  unsigned long nitems_return, bytes_after_return;
  XGetWindowProperty(gc->x.display, win, gc->x.property_atom.qubes_vmname, 0, 256, False,
                     XA_STRING, &type_return, &format,
                     &nitems_return, &bytes_after_return, &vmname);
  // FIXME should XFree
  if (nitems_return == 0)
    return None;
  if (type_return != XA_STRING) {
    LOGE("bad _QUBES_VMNAME property type %lu\n", type_return);
    return None;
  }
  LOGD("format=%u vmname=%s\n", format, vmname);
  return vmname;
}

static Window glse_vmwinid(graphics_context_t* gc, Window win)
{
  Window* vmwinid;
  int format;
  Atom type_return;
  unsigned long nitems_return, bytes_after_return;
  XGetWindowProperty(gc->x.display, win, gc->x.property_atom.qubes_vmwinid, 0, 1, False,
                     XA_WINDOW, &type_return, &format,
                     &nitems_return, &bytes_after_return, (unsigned char**)&vmwinid);
  // FIXME should XFree
  if (nitems_return != 1)
    return None;
  if (type_return != XA_WINDOW) {
    LOGE("bad _QUBES_VMWINDOWID property type %lu\n", type_return);
    return None;
  }
  LOGD("format=%u vmwinid=0x%lx\n", format, *vmwinid);
  return *vmwinid;
}

void glse_handle_x11_event(graphics_context_t* gc, const char* qubes_domain)
{
  while (XPending(gc->x.display)) {
    XEvent evt;
    XNextEvent(gc->x.display, &evt);
    switch (evt.type) {
    case CreateNotify:
      LOGD("CreateNotify 0x%0lx\n", evt.xcreatewindow.window);
      char* vmname = (char*)glse_vmname(gc, evt.xcreatewindow.window);
      if (!vmname) {
        LOGD("ignoring window 0x%0lx, no vmname\n", evt.xcreatewindow.window);
        break;
      }
      LOGD("vmname=%s\n", vmname);
      Window vmwin = glse_vmwinid(gc, evt.xcreatewindow.window);
      LOGD("window=0x%lx\n", vmwin);
      if (strcmp(vmname, qubes_domain) == 0) {
        LOGI("window registered: 0x%lx -> 0x%lx\n", vmwin, evt.xcreatewindow.window);
        list_insert(gc->x.remote2local,
                    vmwin, (void*)evt.xcreatewindow.window);
      }
      else LOGD("ignoring window 0x%0lx, from qube %s\n",
                evt.xcreatewindow.window, vmname);
      break;
    case DestroyNotify:
      LOGD("DestroyNotify 0x%0lx\n", evt.xdestroywindow.window);
      break;
    default:
      // ignore other XEvent types
      ;
      //LOGD("evt type %d\n", evt.type);
    }
  }
}

#else
#error "GLS FIXME: only supporting server on X11 platform"
#endif
