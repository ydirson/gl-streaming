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

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "glcontrol.h"

//#define DEBUG
#define CASE_STRING( value ) case value: return #value; 
const char* eglGetErrorString(EGLint err)
{
    switch(err) {
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
		
		default: return ("EGL_BAD_ERROR_%p", err);
    }
}
const char* glGetErrorString(GLenum err)
{
    switch(err) {
		CASE_STRING(GL_NO_ERROR)
		CASE_STRING(GL_INVALID_ENUM)
		CASE_STRING(GL_INVALID_VALUE)
		CASE_STRING(GL_INVALID_OPERATION)
		CASE_STRING(GL_OUT_OF_MEMORY)
		case 0x8031: /* not core */ return "GL_TABLE_TOO_LARGE_EXT";
		case 0x8065: /* not core */ return "GL_TEXTURE_TOO_LARGE_EXT";
		CASE_STRING(GL_INVALID_FRAMEBUFFER_OPERATION)
		
		default: return("GL_BAD_ERROR_%p", err);
    }
}
#undef CASE_STRING

void base_check_egl_err(char* funcname) {
	int error = eglGetError();
	if (error != EGL_SUCCESS) {
		LOGD("eglGetError(%s) return error %s", funcname, eglGetErrorString(error));
	}
#ifdef DEBUG
	assert(error == 0)
#endif // DEBUG
}
void base_check_gl_err(char* funcname) {
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		LOGD("glGetError(%s) return error %s", funcname, glGetErrorString(error));
	}
#ifdef DEBUG
	assert(error == 0)
#endif // DEBUG
}

void init_egl(graphics_context_t *gc)
{
  EGLBoolean r;
  EGLint num_config;

#ifdef RASPBERRY_PI
  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;
#endif

#ifdef USE_X11
  Window win;
#endif

#ifdef RASPBERRY_PI
  bcm_host_init();
#endif

#ifdef USE_X11
  xDisplay = XOpenDisplay(NULL);
  if (!xDisplay) {
    printf("Error: couldn't open display %s\n", getenv("DISPLAY"));
    exit(EXIT_FAILURE);
	return;
  }
  gc->display = eglGetDisplay(xDisplay);
#else
  gc->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
  assert(gc->display != EGL_NO_DISPLAY);
  check_gl_err(eglGetDisplay);

#ifdef RASPBERRY_PI
  int32_t ri = graphics_get_display_size(0, &gc->screen_width, &gc->screen_height);
  assert(ri >= 0);

  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.width = gc->screen_width;
  dst_rect.height = gc->screen_height;

  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = gc->screen_width << 16;
  src_rect.height = gc->screen_height << 16;

  gc->d_display = vc_dispmanx_display_open(0);
  gc->d_update = vc_dispmanx_update_start(0);
  gc->d_element = vc_dispmanx_element_add(
    gc->d_update, gc->d_display,
    0, &dst_rect, 0,
    &src_rect, DISPMANX_PROTECTION_NONE, 0 , 0, (DISPMANX_TRANSFORM_T)0);
  gc->d_window.element = gc->d_element;
  gc->d_window.width = gc->screen_width;
  gc->d_window.height = gc->screen_height;
  vc_dispmanx_update_submit_sync(gc->d_update);
  check_gl_err();
  gc->surface = eglCreateWindowSurface(gc->display, config, &gc->d_window, NULL);
#elif defined(GLS_SERVER)
  gc->screen_width = glsurfaceview_width; // (gc->d_rect.right - gc->d_rect.left);
  gc->screen_height = glsurfaceview_height; // (gc->d_rect.bottom - gc->d_rect.top);
  // gc->d_window = glsurfaceview_window;
  // assert (glsurfaceview_window != NULL);
#endif
/*
#ifdef __ANDROID__
  assert(gc->d_window != NULL);
#elif defined(USE_X11)
*/

#ifdef USE_X11
  make_egl_base(gc->display, "OpenGL ES 2.x streaming", 0, 0, glsurfaceview_width, glsurfaceview_height, &win, &gc->context, &gc->surface);
  XMapWindow(xDisplay, win);
  // gc->surface = eglCreateWindowSurface(gc->surface, config, win, NULL);
#else
  make_egl_base(gc->display, &gc->context, &gc->surface);
#endif

  if (gc->surface == EGL_NO_SURFACE) {
	  gc->surface = eglGetCurrentSurface(EGL_DRAW);
  }
  assert(gc->surface != EGL_NO_SURFACE);
  check_gl_err(eglGetCurrentSurface);

  r = eglMakeCurrent(gc->display, gc->surface, gc->surface, gc->context);
  assert(EGL_FALSE != r);
  check_gl_err(eglMakeCurrent);
}


void release_egl(graphics_context_t *gc)
{
#ifdef RASPBERRY_PI
  vc_dispmanx_element_remove(gc->d_update, gc->d_element);
  vc_dispmanx_display_close(gc->d_display);
#endif

  eglMakeCurrent(gc->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(gc->display, gc->surface);
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
void make_egl_base(EGLDisplay egl_dpy, const char *name, int x, int y, int width, int height, Window *winRet, EGLContext *ctxRet, EGLSurface *surfRet)
#else
void make_egl_base(EGLDisplay egl_dpy, EGLContext *ctxRet, EGLSurface *surfRet)
#endif
{
   if (!eglInitialize(egl_dpy, NULL, NULL)) {
      printf("Error: eglInitialize() failed\n");
      return -1;
   }
	
   static const EGLint attribs[] = {
      EGL_RED_SIZE, 1,
      EGL_GREEN_SIZE, 1,
      EGL_BLUE_SIZE, 1,
      EGL_DEPTH_SIZE, 1,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
   };
   static const EGLint ctx_attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   
#ifdef USE_X11
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   Window win;
   XVisualInfo *visInfo, visTemplate;
   int num_visuals;
#endif // USE_X11

   EGLContext ctx;
   EGLConfig config;
   EGLint num_configs;
   EGLint vid;

#ifdef USE_X11
   xScreenId = DefaultScreen(xDisplay);
   root = RootWindow( xDisplay, xScreenId );

   /* The X window visual must match the EGL config */
   visTemplate.visualid = XVisualIDFromVisual(XDefaultVisual(xDisplay, xScreenId));
   visInfo = XGetVisualInfo(xDisplay, VisualIDMask, &visTemplate, &num_visuals);
   if (!visInfo) {
      printf("Error: couldn't get X visual\n");
      exit(1);
   }

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap( xDisplay, root, visInfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow( xDisplay, root, 0, 0, width, height,
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
      XSetNormalHints(xDisplay, win, &sizehints);
      XSetStandardProperties(xDisplay, win, name, name,
                              None, (char **)NULL, 0, &sizehints);
   }
#endif // USE_X11

   if (!eglChooseConfig( egl_dpy, attribs, &config, 1, &num_configs)) {
      printf("Error: couldn't get an EGL visual config\n");
      exit(1);
   }
   assert(config);
   assert(num_configs > 0);

   if (!eglGetConfigAttrib(egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
      printf("Error: eglGetConfigAttrib() failed\n");
      exit(1);
   }

   eglBindAPI(EGL_OPENGL_ES_API);

   ctx = eglCreateContext(egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs );
   if (!ctx) {
      printf("Error: eglCreateContext failed\n");
      exit(1);
   }

   // test eglQueryContext() 
   {
      EGLint val;
      eglQueryContext(egl_dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &val);
      // assert(val == 2);
   }
   
#ifdef USE_X11
   *surfRet = eglCreateWindowSurface(egl_dpy, config, win, NULL);
#elif defined(__ANDROID__)
   *surfRet = eglCreateWindowSurface(egl_dpy, config, glsurfaceview_window, NULL);
#else
   printf("FIXME!!! on platform without X11 or Android? Windows?\n");
   exit(1);
#endif

   if (!*surfRet) {
      printf("Error: eglCreateWindowSurface failed: %p\n", eglGetError());
      exit(1);
   }

   // sanity checks
   {
      EGLint val;
      assert(eglGetConfigAttrib(egl_dpy, config, EGL_SURFACE_TYPE, &val));
      assert(val & EGL_WINDOW_BIT);
   }
   
#ifdef USE_X11
   XFree(visInfo);
   *winRet = win;
#endif // USE_X11

   *ctxRet = ctx;
}


