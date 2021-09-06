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

#pragma once

#include "gls_command.h"

#include <stdint.h>
#include <stddef.h>


// EGL 1.0

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint32_t config_size;
  uint32_t has_attribs;
} gls_eglChooseConfig_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t configs[GLS_DATA_SIZE];
  uint32_t num_config;
  uint32_t success;
} gls_ret_eglChooseConfig_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t config;
  uint64_t share_list;
  uint32_t has_attribs;
} gls_eglCreateContext_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t context;
} gls_ret_eglCreateContext_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t config;
  uint32_t has_attribs;
} gls_eglCreatePbufferSurface_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t surface;
} gls_ret_eglCreatePbufferSurface_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t config;
  //uint32_t pixmap; // FIXME
  uint32_t has_attribs;
} gls_eglCreatePixmapSurface_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t surface;
} gls_ret_eglCreatePixmapSurface_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t config;
  uint32_t window;
  uint32_t has_attribs;
} gls_eglCreateWindowSurface_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t surface;
} gls_ret_eglCreateWindowSurface_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t ctx;
} gls_eglDestroyContext_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglDestroyContext_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t surface;
} gls_eglDestroySurface_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglDestroySurface_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t config;
  int32_t attribute;
} gls_eglGetConfigAttrib_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t value;
  uint32_t success;
} gls_ret_eglGetConfigAttrib_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint32_t config_size;
} gls_eglGetConfigs_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t configs[GLS_DATA_SIZE];
  int32_t num_config;
  uint32_t success;
} gls_ret_eglGetConfigs_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_eglGetCurrentDisplay_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t display;
} gls_ret_eglGetCurrentDisplay_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t readdraw;
} gls_eglGetCurrentSurface_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t surface;
} gls_ret_eglGetCurrentSurface_t;


#define GLS_EGL_NATIVE_DISPLAY_DEFAULT 0
#define GLS_EGL_NATIVE_DISPLAY_NATIVE  1

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t native_display;
} gls_eglGetDisplay_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t display;
} gls_ret_eglGetDisplay_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_eglGetError_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t error;
} gls_ret_eglGetError_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  char procname[];
} gls_eglGetProcAddress_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int success;
} gls_ret_eglGetProcAddress_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
} gls_eglInitialize_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t major;
  uint32_t minor;
  uint32_t success;
} gls_ret_eglInitialize_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t draw;
  uint64_t read;
  uint64_t ctx;
} gls_eglMakeCurrent_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglMakeCurrent_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t ctx;
  uint32_t attribute;
} gls_eglQueryContext_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t value;
  uint32_t success;
} gls_ret_eglQueryContext_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint32_t name;
} gls_eglQueryString_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
  char params[GLS_STRING_SIZE_PLUS];
} gls_ret_eglQueryString_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t surface;
  uint32_t attribute;
} gls_eglQuerySurface_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t value;
  uint32_t success;
} gls_ret_eglQuerySurface_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t draw;
} gls_eglSwapBuffers_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglSwapBuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
} gls_eglTerminate_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglTerminate_t;


// EGL 1.1

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t surface;
  uint32_t buffer;
} gls_eglBindTexImage_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglBindTexImage_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t surface;
  uint32_t buffer;
} gls_eglReleaseTexImage_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglReleaseTexImage_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint64_t surface;
  uint32_t attribute;
  uint32_t value;
} gls_eglSurfaceAttrib_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglSurfaceAttrib_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t dpy;
  uint32_t interval;
} gls_eglSwapInterval_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglSwapInterval_t;


// EGL 1.2

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t api;
} gls_eglBindAPI_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglBindAPI_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_eglReleaseThread_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_eglReleaseThread_t;


// EGL 1.4

typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_eglGetCurrentContext_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint64_t context;
} gls_ret_eglGetCurrentContext_t;
