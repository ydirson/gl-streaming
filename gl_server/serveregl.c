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

static const char* GLS_EGL_EXTENSIONS[] =
  {
   "EGL_KHR_config_attribs",        // 1
   "EGL_EXT_platform_base",         // 57
   "EGL_KHR_context_flush_control", // 102
   NULL,
  };

// EGL 1.0

void glse_eglChooseConfig(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglChooseConfig);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_attribs);

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
    success = eglChooseConfig((EGLDisplay)c->dpy, dat,
                              configs, c->config_size, &num_config);
  GLSE_RELEASE_DATA();
  GLSE_SET_RET_PTR(ret, eglChooseConfig);
  if (success && c->config_size)
    memcpy(ret->configs, configs, num_config * sizeof(EGLConfig));
  ret->success = success;
  ret->num_config = num_config;
  GLSE_SEND_RET(ret, eglChooseConfig);
}

void glse_eglCreateContext(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreateContext);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_attribs);

  EGLContext context = eglCreateContext((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                        (EGLContext)c->share_list, dat);

  GLSE_RELEASE_DATA();
  GLSE_SET_RET_PTR(ret, eglCreateContext);
  ret->context = (uint64_t)context;
  GLSE_SEND_RET(ret, eglCreateContext);
}

void glse_eglCreatePbufferSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreatePbufferSurface);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_attribs);

  EGLSurface surface = eglCreatePbufferSurface((EGLDisplay)c->dpy, (EGLConfig)c->config, dat);
  GLSE_RELEASE_DATA();
  GLSE_SET_RET_PTR(ret, eglCreatePbufferSurface);
  ret->surface = (uint64_t)surface;
  GLSE_SEND_RET(ret, eglCreatePbufferSurface);
}

#if 0
void glse_eglCreatePixmapSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreatePixmapSurface);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_attribs);

  // FIXME must transfer Pixmap first
  EGLSurface surface = eglCreatePbufferSurface((EGLDisplay)c->dpy, (EGLConfig)c->config, dat);
  EGLSurface surface = eglGetCurrentSurface(EGL_DRAW); // Stub: current
  GLSE_RELEASE_DATA();
  GLSE_SET_RET_PTR(ret, eglCreatePixmapSurface);
  ret->surface = surface;
  GLSE_SEND_RET(ret, eglCreatePixmapSurface);
}
#endif

void glse_eglCreateWindowSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglCreateWindowSurface);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_attribs);

  EGLSurface surface = eglCreateWindowSurface((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                              glsec_global.gc->x.window, dat);
  GLSE_RELEASE_DATA();
  GLSE_SET_RET_PTR(ret, eglCreateWindowSurface);
  ret->surface = (uint64_t)surface;
  GLSE_SEND_RET(ret, eglCreateWindowSurface);
}

void glse_eglDestroyContext(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglDestroyContext);
  EGLBoolean success = eglDestroyContext((EGLDisplay)c->dpy, (EGLContext)c->ctx);

  GLSE_SET_RET_PTR(ret, eglDestroyContext);
  ret->success = success;
  GLSE_SEND_RET(ret, eglDestroyContext);
}

void glse_eglDestroySurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglDestroySurface);
  EGLBoolean success = eglDestroySurface((EGLDisplay)c->dpy, (EGLSurface)c->surface);

  GLSE_SET_RET_PTR(ret, eglDestroySurface);
  ret->success = success;
  GLSE_SEND_RET(ret, eglDestroySurface);
}

void glse_eglGetConfigAttrib(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetConfigAttrib);
  GLSE_SET_RET_PTR(ret, eglGetConfigAttrib);

  EGLBoolean success = eglGetConfigAttrib((EGLDisplay)c->dpy, (EGLConfig)c->config,
                                          c->attribute, &ret->value);

  ret->success = success;
  GLSE_SEND_RET(ret, eglGetConfigAttrib);
}

void glse_eglGetConfigs(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetConfigs);
  GLSE_SET_RET_PTR(ret, eglGetConfigs);

  EGLBoolean success = eglGetConfigs((EGLDisplay)c->dpy, (EGLConfig*)ret->configs, c->config_size, &ret->num_config);

  ret->success = success;
  GLSE_SEND_RET(ret, eglGetConfigs);
}

void glse_eglGetCurrentDisplay(gls_command_t* buf)
{
  (void)buf;
  EGLDisplay display = eglGetCurrentDisplay();

  GLSE_SET_RET_PTR(ret, eglGetCurrentDisplay);
  ret->display = (uint64_t)display;
  GLSE_SEND_RET(ret, eglGetCurrentDisplay);
}

void glse_eglGetCurrentSurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglGetCurrentSurface);
  EGLSurface surface = eglGetCurrentSurface(c->readdraw);

  GLSE_SET_RET_PTR(ret, eglGetCurrentSurface);
  ret->surface = (uint64_t)surface;
  GLSE_SEND_RET(ret, eglGetCurrentSurface);
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

  GLSE_SET_RET_PTR(ret, eglGetDisplay);
  ret->display = (uint64_t)display;
  GLSE_SEND_RET(ret, eglGetDisplay);
}

void glse_eglGetError(gls_command_t* buf)
{
  (void)buf;
  EGLint error = eglGetError();
  // Should check gl error inside eglGetError() ???

  GLSE_SET_RET_PTR(ret, eglGetError);
  ret->error = error;
  GLSE_SEND_RET(ret, eglGetError);
}

void glse_eglInitialize(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglInitialize);
  GLSE_SET_RET_PTR(ret, eglInitialize);
  EGLint major, minor;
  EGLBoolean success = eglInitialize((EGLDisplay)c->dpy, &major, &minor);

  // FIXME EGL_VERSION should be generated from this
  if (major > 1 || minor > 4) minor = 4;
  if (major > 1) major = 1;

  ret->major = major;
  ret->minor = minor;
  ret->success = success;
  GLSE_SEND_RET(ret, eglInitialize);
}

void glse_eglMakeCurrent(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglMakeCurrent);
  EGLBoolean success = eglMakeCurrent((EGLDisplay)c->dpy, (EGLSurface)c->draw,
                                      (EGLSurface)c->read, (EGLContext)c->ctx);

  GLSE_SET_RET_PTR(ret, eglMakeCurrent);
  ret->success = success;
  GLSE_SEND_RET(ret, eglMakeCurrent);
}

void glse_eglQueryContext(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglQueryContext);
  GLSE_SET_RET_PTR(ret, eglQueryContext);

  EGLBoolean success = eglQueryContext((EGLDisplay)c->dpy, (EGLContext)c->ctx,
                                       c->attribute, &ret->value);

  ret->success = success;
  GLSE_SEND_RET(ret, eglQueryContext);
}

void glse_eglQueryString(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglQueryString);
  GLSE_SET_RET_PTR(ret, eglQueryString);

  const char* params = eglQueryString((EGLDisplay)c->dpy, c->name);

  switch (c->name) {
  case EGL_EXTENSIONS: {
      size_t outlen = 0;
      while (1) {
        size_t len = strcspn(params, " ");
        if (!len) break;
        if (glse_extension_supported(GLS_EGL_EXTENSIONS, params, len)) {
          if (outlen + len + 1 > GLSE_OUT_BUFFER_SIZE - sizeof(gls_ret_eglQueryString_t)) {
            fprintf(stderr,
                    "GLS WARNING: %s: not enough buffer space for all extensions, truncating\n",
                    __FUNCTION__);
            break;
          }
          strncpy(ret->params + outlen, params, len);
          ret->params[outlen + len] = ' ';
          outlen += len + 1;
        }
        // skip this ext and any sep
        params += len;
        params += strspn(params, " ");
      }
      if (outlen)
        // overwrite last space
        ret->params[outlen - 1] = '\0';
      else
        ret->params[0] = '\0';
      break;
    }
  case EGL_CLIENT_APIS:
    // FIXME would rather query and filter those we support
    strcpy(ret->params, "OpenGL_ES");
    ret->success = TRUE;
    break;
  case EGL_VERSION:
    // wrong but glmark2 wont work without 1.4
    strcpy(ret->params, "1.4 GLS"); // FIXME should match eglInitialize
    ret->success = TRUE;
    break;
  case EGL_VENDOR: {
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

  // LOGD("Client asking for 0x%04x, return %s\n", c->name, params);
  // ret->params[GLS_STRING_SIZE_PLUS - 1] = '\0';
  GLSE_SEND_RET(ret, eglQueryString);
}

void glse_eglQuerySurface(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglQuerySurface);
  GLSE_SET_RET_PTR(ret, eglQuerySurface);

  EGLBoolean success = eglQuerySurface((EGLDisplay)c->dpy, (EGLSurface)c->surface,
                                       c->attribute, &ret->value);

  ret->success = success;
  GLSE_SEND_RET(ret, eglQuerySurface);
}

void glse_eglSwapBuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglSwapBuffers);
  GLSE_SET_RET_PTR(ret, eglSwapBuffers);
  ret->success = eglSwapBuffers((EGLDisplay)c->dpy, (EGLSurface)c->draw);
  GLSE_SEND_RET(ret, eglSwapBuffers);
}

void glse_eglTerminate(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglTerminate);
  GLSE_SET_RET_PTR(ret, eglTerminate);
  ret->success = eglTerminate((EGLDisplay)c->dpy);
  GLSE_SEND_RET(ret, eglTerminate);
}

// EGL 1.1

void glse_eglSwapInterval(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglSwapInterval);
  GLSE_SET_RET_PTR(ret, eglSwapInterval);
  ret->success = eglSwapInterval((EGLDisplay)c->dpy, c->interval);
  GLSE_SEND_RET(ret, eglSwapInterval);
}

// EGL 1.2

void glse_eglBindAPI(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, eglBindAPI);
  EGLBoolean success = eglBindAPI(c->api);

  GLSE_SET_RET_PTR(ret, eglBindAPI);
  ret->success = success;
  GLSE_SEND_RET(ret, eglBindAPI);
}

void glse_eglReleaseThread(gls_command_t* buf)
{
  (void)buf;
  EGLBoolean success = eglReleaseThread();

  GLSE_SET_RET_PTR(ret, eglReleaseThread);
  ret->success = success;
  GLSE_SEND_RET(ret, eglReleaseThread);
}

// EGL 1.4

void glse_eglGetCurrentContext(gls_command_t* buf)
{
  (void)buf;
  EGLContext context = eglGetCurrentContext();

  GLSE_SET_RET_PTR(ret, eglGetCurrentContext);
  ret->context = (uint64_t)context;
  GLSE_SEND_RET(ret, eglGetCurrentContext);
}

//

#define CASE_EXEC_CMD(FUNCNAME) \
  case GLSC_##FUNCNAME: glse_##FUNCNAME(c); break

int egl_executeCommand(gls_command_t* c)
{
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
    CASE_EXEC_CMD(eglSwapInterval);
// EGL 1.2
    CASE_EXEC_CMD(eglBindAPI);
    //CASE_EXEC_CMD(eglQueryAPI);
    //CASE_EXEC_CMD(eglCreatePbufferFromClientBuffer);
    CASE_EXEC_CMD(eglReleaseThread);
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
