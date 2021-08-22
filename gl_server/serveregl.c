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

// This file declare EGL methods on server side

#include "gls_command_egl.h"
#include "glserver.h"
#include "fastlog.h"

#include <alloca.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG

// EGL 1.0

void glse_eglChooseConfig(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglChooseConfig);
  EGLint *attrib_list = NULL;
  GLSE_SET_DATA_PTR(dat, egl_attriblist, c->has_attribs);
  attrib_list = dat->attrib_list;

  EGLint num_config;
  EGLConfig* configs = NULL;
  EGLBoolean success = EGL_TRUE;
  if (c->config_size) { // our way of passing configs=NULL
    configs = alloca(c->config_size * sizeof(EGLConfig));
    if (!configs) {
      LOGE("eglChooseConfig: failed to allocate space for %d configs\n", c->config_size);
      success = EGL_FALSE;
      num_config = 0;
    }
  }

  if (success)
    success = eglChooseConfig((EGLDisplay)c->dpy, attrib_list,
                              configs, c->config_size, &num_config);
  gls_ret_eglChooseConfig_t *ret = (gls_ret_eglChooseConfig_t *)glsec_global.pool.tmp_buf.buf;
  if (success && c->config_size)
      memcpy(ret->configs, configs, num_config * sizeof(EGLConfig));
  ret->success = success;
  ret->num_config = num_config;
  ret->cmd = GLSC_eglChooseConfig;
  glse_cmd_send_data(sizeof(gls_ret_eglChooseConfig_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglCreateContext(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreateContext);
  EGLint *attrib_list = NULL;
  GLSE_SET_DATA_PTR(dat, egl_attriblist, c->has_attribs);
  attrib_list = dat->attrib_list;

  EGLContext context = eglCreateContext((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                        (EGLContext)c->share_list, attrib_list);

  gls_ret_eglCreateContext_t *ret = (gls_ret_eglCreateContext_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglCreateContext;
  ret->context = (uint64_t)context;
  glse_cmd_send_data(sizeof(gls_ret_eglCreateContext_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglCreatePbufferSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreatePbufferSurface);
  EGLint *attrib_list = NULL;
  GLSE_SET_DATA_PTR(dat, egl_attriblist, c->has_attribs);
  attrib_list = dat->attrib_list;

  EGLSurface surface = eglCreatePbufferSurface((EGLDisplay)c->dpy, (EGLConfig)c->config, attrib_list);
  gls_ret_eglCreatePbufferSurface_t *ret = (gls_ret_eglCreatePbufferSurface_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglCreatePbufferSurface;
  ret->surface = (uint64_t)surface;
  glse_cmd_send_data(sizeof(gls_ret_eglCreatePbufferSurface_t), (char *)glsec_global.pool.tmp_buf.buf);
}

#if 0
void glse_eglCreatePixmapSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreatePixmapSurface);
  EGLint *attrib_list = NULL;
  GLSE_SET_DATA_PTR(dat, egl_attriblist, c->has_attribs);
  attrib_list = dat->attrib_list;

  // FIXME must transfer Pixmap first
  EGLSurface surface = eglCreatePbufferSurface((EGLDisplay)c->dpy, (EGLConfig)c->config, attrib_list);
  EGLSurface surface = eglGetCurrentSurface(EGL_DRAW); // Stub: current
  gls_ret_eglCreatePixmapSurface_t *ret = (gls_ret_eglCreatePixmapSurface_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglCreatePixmapSurface;
  ret->surface = surface;
  glse_cmd_send_data(sizeof(gls_ret_eglCreatePixmapSurface_t), (char *)glsec_global.pool.tmp_buf.buf);
}
#endif

void glse_eglCreateWindowSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreateWindowSurface);
  EGLint *attrib_list = NULL;
  GLSE_SET_DATA_PTR(dat, egl_attriblist, c->has_attribs);
  attrib_list = dat->attrib_list;

  fprintf(stderr, "GLS WARNING: eglCreateWindowSurface ignoring window parameter\n");
  EGLSurface surface = eglCreateWindowSurface((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                              glsec_global.gc->x.window, // FIXME
                                              attrib_list);
  gls_ret_eglCreateWindowSurface_t *ret = (gls_ret_eglCreateWindowSurface_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglCreateWindowSurface;
  ret->surface = (uint64_t)surface;
  glse_cmd_send_data(sizeof(gls_ret_eglCreateWindowSurface_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglDestroyContext(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglDestroyContext);
  EGLBoolean success = eglDestroyContext((EGLDisplay)c->dpy, (EGLContext)c->ctx);

  gls_ret_eglDestroyContext_t *ret = (gls_ret_eglDestroyContext_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglDestroyContext;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglDestroyContext_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglDestroySurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglDestroySurface);
  EGLBoolean success = eglDestroySurface((EGLDisplay)c->dpy, (EGLSurface)c->surface);

  gls_ret_eglDestroySurface_t *ret = (gls_ret_eglDestroySurface_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglDestroySurface;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglDestroySurface_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglGetConfigAttrib(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetConfigAttrib);
  gls_ret_eglGetConfigAttrib_t *ret = (gls_ret_eglGetConfigAttrib_t *)glsec_global.pool.tmp_buf.buf;
  
  EGLBoolean success = eglGetConfigAttrib((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                          c->attribute, &ret->value);

  ret->cmd = GLSC_eglGetConfigAttrib;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglGetConfigAttrib_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglGetConfigs(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetConfigs);
  gls_ret_eglGetConfigs_t *ret = (gls_ret_eglGetConfigs_t *)glsec_global.pool.tmp_buf.buf;
  
  EGLBoolean success = eglGetConfigs((EGLDisplay)c->dpy, (EGLConfig*)ret->configs, c->config_size, &ret->num_config);
  
  ret->cmd = GLSC_eglGetConfigs;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglGetConfigs_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglGetCurrentDisplay(gls_command_t* buf)
{
  (void)buf;
  EGLDisplay display = eglGetCurrentDisplay();
  
  gls_ret_eglGetCurrentDisplay_t *ret = (gls_ret_eglGetCurrentDisplay_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglGetCurrentDisplay;
  ret->display = (uint64_t)display;
  glse_cmd_send_data(sizeof(gls_ret_eglGetCurrentDisplay_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglGetCurrentSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetCurrentSurface);
  EGLSurface surface = eglGetCurrentSurface(c->readdraw);
  
  gls_ret_eglGetCurrentSurface_t *ret = (gls_ret_eglGetCurrentSurface_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglGetCurrentSurface;
  ret->surface = (uint64_t)surface;
  glse_cmd_send_data(sizeof(gls_ret_eglGetCurrentSurface_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglGetDisplay(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetDisplay);
  EGLDisplay display;
  if (c->native_display != (uint64_t)EGL_DEFAULT_DISPLAY) {
    fprintf(stderr, "eglGetDisplay: ERROR, only supports EGL_DEFAULT_DISPLAY\n");
    display = EGL_NO_DISPLAY;
  } else {
    if (0)
      display = eglGetDisplay((EGLNativeDisplayType)c->native_display);
    else {
      // FIXME stub to remove when eglInitialize gets unstubbed
      fprintf(stderr, "eglGetDisplay: faking for single-window mode\n");
      display = glsec_global.gc->display;
    }
  }

  // FIXME should keep track of EGLDisplay values the client is allowed to use

  gls_ret_eglGetDisplay_t *ret = (gls_ret_eglGetDisplay_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglGetDisplay;
  ret->display = (uint64_t)display;
  glse_cmd_send_data(sizeof(gls_ret_eglGetDisplay_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglGetError(gls_command_t* buf)
{
  (void)buf;
  GLuint error = eglGetError();
  // Should check gl error inside eglGetError() ???
  
  gls_ret_eglGetError_t *ret = (gls_ret_eglGetError_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglGetError;
  ret->error = error;
  glse_cmd_send_data(sizeof(gls_ret_eglGetError_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglInitialize(gls_command_t* buf)
{
  (void)buf;
  EGLBoolean success = EGL_TRUE; // Current stub instead of real init
  // GLSE_SET_COMMAND_PTR(c, eglInitialize);
  // eglInitialize((EGLDisplay)c->dpy, c->major, c->minor);
  
  gls_ret_eglInitialize_t *ret = (gls_ret_eglInitialize_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglInitialize;
  ret->major = 1; ret->minor = 4; // matches EGL_VERSION
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglInitialize_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglMakeCurrent(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglMakeCurrent);
  EGLBoolean success = eglMakeCurrent((EGLDisplay)c->dpy, (EGLSurface)c->draw,
                                      (EGLSurface)c->read, (EGLContext)c->ctx);

  gls_ret_eglMakeCurrent_t *ret = (gls_ret_eglMakeCurrent_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglMakeCurrent;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglMakeCurrent_t), (char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglQueryContext(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglQueryContext);
  gls_ret_eglQueryContext_t *ret = (gls_ret_eglQueryContext_t *)glsec_global.pool.tmp_buf.buf;
  
  EGLBoolean success = eglQueryContext((EGLDisplay)c->dpy, (EGLContext)c->ctx,
                                       c->attribute, &ret->value);
  
  ret->cmd = GLSC_eglQueryContext;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglQueryContext_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglQueryString(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglQueryString);
  gls_ret_eglQueryString_t *ret = (gls_ret_eglQueryString_t *)glsec_global.pool.tmp_buf.buf;
  
  switch (c->name) {
  case EGL_EXTENSIONS:
    // we don't support any right now
    // FIXME later will need to query and filter those we support
    strcpy(ret->params, "EGL_GLS_dummy");
    ret->success = TRUE;
    break;
  case EGL_CLIENT_APIS:
    // FIXME would rather query and filter those we support
    strcpy(ret->params, "OpenGL_ES");
    ret->success = TRUE;
    break;
  case EGL_VERSION:
    // wrong but glmark2 wont work without 1.4
    strcpy(ret->params, "1.4 GLS"); // matches eglInitialize
    ret->success = TRUE;
    break;
  case EGL_VENDOR:
    {
      const char *params = eglQueryString((EGLDisplay)c->dpy, c->name);
      const char prefix[] = "GLS + ";
      if (params) {
        strcpy(ret->params, prefix);
        strncpy(ret->params + sizeof(prefix) - 1, params, GLS_STRING_SIZE - sizeof(prefix));
        ret->success = TRUE;
      } else
        ret->success = FALSE;
    }
    break;
  default:
    fprintf(stderr, "GLS WARNING: eglQueryString: unsupported string name 0x%x\n", c->name);
    ret->success = FALSE;
  }

  ret->cmd = GLSC_eglQueryString;
  // LOGD("Client asking for 0x%04x, return %s\n", c->name, params);
  // ret->params[GLS_STRING_SIZE_PLUS - 1] = '\0';
  glse_cmd_send_data(sizeof(gls_ret_eglQueryString_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglQuerySurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglQuerySurface);
  gls_ret_eglQuerySurface_t *ret = (gls_ret_eglQuerySurface_t *)glsec_global.pool.tmp_buf.buf;
  
  EGLBoolean success = eglQuerySurface((EGLDisplay)c->dpy, (EGLSurface)c->surface,
                                       c->attribute, &ret->value);
  
  ret->cmd = GLSC_eglQuerySurface;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglQuerySurface_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglSwapBuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglSwapBuffers);
  gls_ret_eglSwapBuffers_t *ret = (gls_ret_eglSwapBuffers_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglSwapBuffers;
  ret->success = eglSwapBuffers((EGLDisplay)c->dpy, (EGLSurface)c->draw);
  glse_cmd_send_data(sizeof(gls_ret_eglSwapBuffers_t),(char *)glsec_global.pool.tmp_buf.buf);
}

void glse_eglTerminate(gls_command_t* buf)
{
  (void)buf;
  gls_ret_eglTerminate_t *ret = (gls_ret_eglTerminate_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglTerminate;
  ret->success = EGL_TRUE; // Current stub instead of real init
  // GLSE_SET_COMMAND_PTR(c, eglTerminate);
  // eglTerminate((EGLDisplay)c->dpy);
  
  glse_cmd_send_data(sizeof(gls_ret_eglTerminate_t),(char *)glsec_global.pool.tmp_buf.buf);
}

// EGL 1.1

// EGL 1.2

void glse_eglBindAPI(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglBindAPI);
  EGLBoolean success = eglBindAPI(c->api);
  
  gls_ret_eglBindAPI_t *ret = (gls_ret_eglBindAPI_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglBindAPI;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_eglBindAPI_t),(char *)glsec_global.pool.tmp_buf.buf);
}

// EGL 1.4

void glse_eglGetCurrentContext(gls_command_t* buf)
{
  (void)buf;
  EGLContext context = eglGetCurrentContext();
  
  gls_ret_eglGetCurrentContext_t *ret = (gls_ret_eglGetCurrentContext_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_eglGetCurrentContext;
  ret->context = (uint64_t)context;
  glse_cmd_send_data(sizeof(gls_ret_eglGetCurrentContext_t), (char *)glsec_global.pool.tmp_buf.buf);
}

//

#define CASE_EXEC_CMD(FUNCNAME) \
  case GLSC_##FUNCNAME: glse_##FUNCNAME(c); break

int egl_executeCommand(gls_command_t *c) {
#ifdef DEBUG
    LOGD("egl_executeCommand: Executing command 0x%x (%s)\n", c->cmd, GLSC_tostring(c->cmd));
#endif
    switch (c->cmd) {
// EGL 1.0
        CASE_EXEC_CMD(eglChooseConfig);
        //CASE_EXEC_CMD(eglCopyBuffers);
        CASE_EXEC_CMD(eglCreateContext);
        CASE_EXEC_CMD(eglCreatePbufferSurface);
        //CASE_EXEC_CMD(eglCreatePixmapSurface);
        CASE_EXEC_CMD(eglCreateWindowSurface);
        CASE_EXEC_CMD(eglDestroyContext);
        CASE_EXEC_CMD(eglDestroySurface);
        CASE_EXEC_CMD(eglGetConfigAttrib);
        CASE_EXEC_CMD(eglGetConfigs);
        CASE_EXEC_CMD(eglGetCurrentDisplay);
        CASE_EXEC_CMD(eglGetCurrentSurface);
        CASE_EXEC_CMD(eglGetDisplay);
        CASE_EXEC_CMD(eglGetError);
        //CASE_EXEC_CMD(eglGetProcAddress);
        CASE_EXEC_CMD(eglInitialize);
        CASE_EXEC_CMD(eglMakeCurrent);
        CASE_EXEC_CMD(eglQueryContext);
        CASE_EXEC_CMD(eglQueryString);
        CASE_EXEC_CMD(eglQuerySurface);
        CASE_EXEC_CMD(eglSwapBuffers);
        CASE_EXEC_CMD(eglTerminate);
        //CASE_EXEC_CMD(eglWaitGL);
        //CASE_EXEC_CMD(eglWaitNative);
// EGL 1.1
        //CASE_EXEC_CMD(eglBindTexImage);
        //CASE_EXEC_CMD(eglReleaseTexImage);
        //CASE_EXEC_CMD(eglSurfaceAttrib);
        //CASE_EXEC_CMD(eglSwapInterval);
// EGL 1.2
        CASE_EXEC_CMD(eglBindAPI);
        //CASE_EXEC_CMD(eglQueryAPI);
        //CASE_EXEC_CMD(eglCreatePbufferFromClientBuffer);
        //CASE_EXEC_CMD(eglReleaseThread);
        //CASE_EXEC_CMD(eglWaitClient);
// EGL 1.4
        CASE_EXEC_CMD(eglGetCurrentContext);
// EGL 1.5
        //CASE_EXEC_CMD(eglCreateSync);
        //CASE_EXEC_CMD(eglDestroySync);
        //CASE_EXEC_CMD(eglClientWaitSync);
        //CASE_EXEC_CMD(eglGetSyncAttrib);
        //CASE_EXEC_CMD(eglCreateImage);
        //CASE_EXEC_CMD(eglDestroyImage);
        //CASE_EXEC_CMD(eglGetPlatformDisplay);
        //CASE_EXEC_CMD(eglCreatePlatformPixmapSurface);
        //CASE_EXEC_CMD(eglCreatePlatformWindowSurface);
        //CASE_EXEC_CMD(eglWaitSync);
    default:
        return FALSE;
    }
    
    check_egl_err_cmd(c->cmd);
    return TRUE;
}

#undef CASE_EXEC_CMD
