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

#include "glserver.h"
#include <string.h>
#include <alloca.h>

// EGL 1.0

void glse_eglChooseConfig()
{
  GLSE_SET_COMMAND_PTR(c, eglChooseConfig);
  gls_data_egl_attriblist_t *dat = (gls_data_egl_attriblist_t *)glsec_global.tmp_buf.buf;
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
    success = eglChooseConfig((EGLDisplay)c->dpy, dat->attrib_list,
                              configs, c->config_size, &num_config);
  gls_ret_eglChooseConfig_t *ret = (gls_ret_eglChooseConfig_t *)glsec_global.tmp_buf.buf;
  if (success && c->config_size)
      memcpy(ret->configs, configs, num_config * sizeof(EGLConfig));
  ret->success = success;
  ret->num_config = num_config;
  ret->cmd = GLSC_eglChooseConfig;
  glse_cmd_send_data(0,sizeof(gls_ret_eglChooseConfig_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglGetConfigAttrib()
{
  GLSE_SET_COMMAND_PTR(c, eglGetConfigAttrib);
  gls_ret_eglGetConfigAttrib_t *ret = (gls_ret_eglGetConfigAttrib_t *)glsec_global.tmp_buf.buf;
  
  EGLBoolean success = eglGetConfigAttrib((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                          c->attribute, &ret->value);

  ret->cmd = GLSC_eglGetConfigAttrib;
  ret->success = success;
  glse_cmd_send_data(0,sizeof(gls_ret_eglGetConfigAttrib_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglGetConfigs()
{
  GLSE_SET_COMMAND_PTR(c, eglGetConfigs);
  gls_ret_eglGetConfigs_t *ret = (gls_ret_eglGetConfigs_t *)glsec_global.tmp_buf.buf;
  
  EGLBoolean success = eglGetConfigs((EGLDisplay)c->dpy, (EGLConfig*)ret->configs, c->config_size, &ret->num_config);
  
  ret->cmd = GLSC_eglGetConfigs;
  ret->success = success;
  glse_cmd_send_data(0,sizeof(gls_ret_eglGetConfigs_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglGetCurrentDisplay()
{
  EGLDisplay display = eglGetCurrentDisplay();
  
  gls_ret_eglGetCurrentDisplay_t *ret = (gls_ret_eglGetCurrentDisplay_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglGetCurrentDisplay;
  ret->display = (uint64_t)display;
  glse_cmd_send_data(0, sizeof(gls_ret_eglGetCurrentDisplay_t), (char *)glsec_global.tmp_buf.buf);
}

void glse_eglGetCurrentSurface()
{
  GLSE_SET_COMMAND_PTR(c, eglGetCurrentSurface);
  EGLSurface surface = eglGetCurrentSurface(c->readdraw);
  
  gls_ret_eglGetCurrentSurface_t *ret = (gls_ret_eglGetCurrentSurface_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglGetCurrentSurface;
  ret->surface = (uint64_t)surface;
  glse_cmd_send_data(0, sizeof(gls_ret_eglGetCurrentSurface_t), (char *)glsec_global.tmp_buf.buf);
}

void glse_eglGetDisplay()
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

  gls_ret_eglGetDisplay_t *ret = (gls_ret_eglGetDisplay_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglGetDisplay;
  ret->display = (uint64_t)display;
  glse_cmd_send_data(0,sizeof(gls_ret_eglGetDisplay_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglGetError()
{
  GLuint error = eglGetError();
  // Should check gl error inside eglGetError() ???
  
  gls_ret_eglGetError_t *ret = (gls_ret_eglGetError_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglGetError;
  ret->error = error;
  glse_cmd_send_data(0, sizeof(gls_ret_eglGetError_t), (char *)glsec_global.tmp_buf.buf);
}

void glse_eglInitialize()
{
  EGLBoolean success = EGL_TRUE; // Current stub instead of real init
  // GLSE_SET_COMMAND_PTR(c, eglInitialize);
  // eglInitialize((EGLDisplay)c->dpy, c->major, c->minor);
  
  gls_ret_eglInitialize_t *ret = (gls_ret_eglInitialize_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglInitialize;
  ret->major = 1; ret->minor = 0; // FIXME partially reflects amount of stubbing
  ret->success = success;
  glse_cmd_send_data(0,sizeof(gls_ret_eglInitialize_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglQueryContext()
{
  GLSE_SET_COMMAND_PTR(c, eglQueryContext);
  gls_ret_eglQueryContext_t *ret = (gls_ret_eglQueryContext_t *)glsec_global.tmp_buf.buf;
  
  EGLBoolean success = eglQueryContext((EGLDisplay)c->dpy, (EGLContext)c->ctx,
                                       c->attribute, &ret->value);
  
  ret->cmd = GLSC_eglQueryContext;
  ret->success = success;
  glse_cmd_send_data(0,sizeof(gls_ret_eglQueryContext_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglQueryString()
{
  GLSE_SET_COMMAND_PTR(c, eglQueryString);
  gls_ret_eglQueryString_t *ret = (gls_ret_eglQueryString_t *)glsec_global.tmp_buf.buf;
  
  switch (c->name) {
  case EGL_EXTENSIONS:
    // we don't support any right now
    // FIXME later will need to query and filter those we support
    strcpy(ret->params, "");
    ret->success = TRUE;
    break;
  case EGL_CLIENT_APIS:
    // FIXME would rather query and filter those we support
    strcpy(ret->params, "OpenGL_ES");
    ret->success = TRUE;
    break;
  default:
    // EGL_VENDOR, EGL_VERSION: query
    {
      const char *params = eglQueryString((EGLDisplay)c->dpy, c->name);
      if (params) {
        strncpy(ret->params, params, GLS_STRING_SIZE);
        ret->success = TRUE;
      } else
        ret->success = FALSE;
    }
  }

  ret->cmd = GLSC_eglQueryString;
  // LOGD("Client asking for 0x%04x, return %s\n", c->name, params);
  // ret->params[GLS_STRING_SIZE_PLUS - 1] = '\0';
  glse_cmd_send_data(0,sizeof(gls_ret_eglQueryString_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglQuerySurface()
{
  GLSE_SET_COMMAND_PTR(c, eglQuerySurface);
  gls_ret_eglQuerySurface_t *ret = (gls_ret_eglQuerySurface_t *)glsec_global.tmp_buf.buf;
  
  EGLBoolean success = eglQuerySurface((EGLDisplay)c->dpy, (EGLSurface)c->surface,
                                       c->attribute, &ret->value);
  
  ret->cmd = GLSC_eglQuerySurface;
  ret->success = success;
  glse_cmd_send_data(0,sizeof(gls_ret_eglQuerySurface_t),(char *)glsec_global.tmp_buf.buf);
}

void glse_eglTerminate()
{
  gls_ret_eglTerminate_t *ret = (gls_ret_eglTerminate_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglTerminate;
  ret->success = EGL_TRUE; // Current stub instead of real init
  // GLSE_SET_COMMAND_PTR(c, eglTerminate);
  // eglTerminate((EGLDisplay)c->dpy);
  
  glse_cmd_send_data(0,sizeof(gls_ret_eglTerminate_t),(char *)glsec_global.tmp_buf.buf);
}

// EGL 1.1

// EGL 1.2

void glse_eglBindAPI()
{
  GLSE_SET_COMMAND_PTR(c, eglBindAPI);
  EGLBoolean success = eglBindAPI(c->api);
  
  gls_ret_eglBindAPI_t *ret = (gls_ret_eglBindAPI_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglBindAPI;
  ret->success = success;
  glse_cmd_send_data(0,sizeof(gls_ret_eglBindAPI_t),(char *)glsec_global.tmp_buf.buf);
}

// EGL 1.4

void glse_eglGetCurrentContext()
{
  EGLContext context = eglGetCurrentContext();
  
  gls_ret_eglGetCurrentContext_t *ret = (gls_ret_eglGetCurrentContext_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_eglGetCurrentContext;
  ret->context = (uint64_t)context;
  glse_cmd_send_data(0, sizeof(gls_ret_eglGetCurrentContext_t), (char *)glsec_global.tmp_buf.buf);
}

//

#define CASE_EXEC_CMD(FUNCNAME) case GLSC_##FUNCNAME: glse_##FUNCNAME(); break

int egl_executeCommand(gls_command_t *c) {
#ifdef DEBUG
    LOGD("egl_executeCommand: Executing command %d (%s)\n", c->cmd, GLSC_tostring(c->cmd));
#endif
    switch (c->cmd) {
// EGL 1.0
        CASE_EXEC_CMD(eglChooseConfig);
        //CASE_EXEC_CMD(eglCopyBuffers);
        //CASE_EXEC_CMD(eglCreateContext);
        //CASE_EXEC_CMD(eglCreatePbufferSurface);
        //CASE_EXEC_CMD(eglCreatePixmapSurface);
        //CASE_EXEC_CMD(eglCreateWindowSurface);
        //CASE_EXEC_CMD(eglDestroyContext);
        //CASE_EXEC_CMD(eglDestroySurface);
        CASE_EXEC_CMD(eglGetConfigAttrib);
        CASE_EXEC_CMD(eglGetConfigs);
        CASE_EXEC_CMD(eglGetCurrentDisplay);
        CASE_EXEC_CMD(eglGetCurrentSurface);
        CASE_EXEC_CMD(eglGetDisplay);
        CASE_EXEC_CMD(eglGetError);
        //CASE_EXEC_CMD(eglGetProcAddress);
        CASE_EXEC_CMD(eglInitialize);
        //CASE_EXEC_CMD(eglMakeCurrent);
        CASE_EXEC_CMD(eglQueryContext);
        CASE_EXEC_CMD(eglQueryString);
        CASE_EXEC_CMD(eglQuerySurface);
        //CASE_EXEC_CMD(eglSwapBuffers);
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

int egl_flushCommand(gls_command_t *c) {
    switch (c->cmd) {
        // Nothing here to flush
        default:
            return FALSE;
    }
    
    check_egl_err_cmd(c->cmd);
    return TRUE;
}

