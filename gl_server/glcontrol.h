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

#include <EGL/egl.h>

extern int   var_server_port;
extern int   var_client_port;
extern const char* var_client_addr;
/*
extern const char* var_path_shadercache;
extern const char* var_file_fragment_shader;
extern const char* var_file_vertex_shader;
*/

typedef struct
{
  EGLDisplay display;
#ifdef USE_X11
  struct
  {
    Display* display;
    Window window;
  } x;
#endif
} graphics_context_t;

#define check_egl_err(FUNCNAME) base_check_egl_err(#FUNCNAME);
#define check_gl_err(FUNCNAME) base_check_gl_err(#FUNCNAME);

#define check_egl_err_cmd(cmd) base_check_egl_err(GLSC_tostring(cmd));
#define check_gl_err_cmd(cmd) base_check_gl_err(GLSC_tostring(cmd));

extern uint32_t last_cmd;
void base_check_egl_err(const char* funcname);
void base_check_gl_err(const char* funcname);

extern EGLConfig config;

void init_egl(graphics_context_t* gc);
void release_egl(graphics_context_t* gc);

#if USE_X11
void gls_create_x11_window(graphics_context_t* gc, const char* name, int x, int y, int width, int height);
#endif
