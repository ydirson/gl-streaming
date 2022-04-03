// This file declare EGL methods for stubs or streaming

#define _GNU_SOURCE
#include "gls_command_egl.h"
#include "glclient.h"

#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Define core API functions as __GLS_##FUNC with FUNC exported as alias
#define GLS_DEF_CORE_API(RET, FUNC, ...)                                \
  __asm__(".global " #FUNC "; .set " #FUNC ", __GLS_"#FUNC);            \
  static EGLAPI RET EGLAPIENTRY __GLS_##FUNC (__VA_ARGS__)

static struct
{
  EGLNativeDisplayType native_display;
#if defined(USE_X11)
  Window x_window;
#endif
} egl_clt_context;

static int gls_wire_native_display(EGLNativeDisplayType native_display,
                                   uint32_t* wire_native_display_p)
{
  if (native_display == EGL_DEFAULT_DISPLAY) {
    *wire_native_display_p = GLS_EGL_NATIVE_DISPLAY_DEFAULT;
  } else {
    if (egl_clt_context.native_display && egl_clt_context.native_display != native_display) {
      LOGE("only support one native display\n");
      return -1;
    }
    egl_clt_context.native_display = native_display;
    *wire_native_display_p = GLS_EGL_NATIVE_DISPLAY_NATIVE;
  }
  return 0;
}

#define SEND_ATTRIB_DATA(FLAG, ATTRIB_LIST)                             \
  uint32_t FLAG;                                                        \
  if (!ATTRIB_LIST || ATTRIB_LIST[0] == EGL_NONE) {                     \
    FLAG = 0;                                                           \
  } else {                                                              \
    unsigned num_attribs;                                               \
    unsigned data_size;                                                 \
    for (num_attribs = 0; ATTRIB_LIST[2 * num_attribs] != EGL_NONE; num_attribs++); \
    data_size = (num_attribs * 2 + 1) * sizeof(EGLint);                 \
    assert(data_size < GLS_DATA_SIZE * sizeof(EGLint));                 \
    memcpy(glsc_global.pool.tmp_buf.buf, ATTRIB_LIST, data_size);       \
    gls_cmd_send_data(data_size, glsc_global.pool.tmp_buf.buf);         \
    FLAG = (num_attribs != 0);                                          \
  }                                                                     \
  //

// EGL 1.0

GLS_DEF_CORE_API(EGLBoolean, eglChooseConfig,  EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config )
{
  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglChooseConfig);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  if (configs)
    c->config_size = config_size;
  else
    c->config_size = 0;
  GLS_SEND_PACKET(eglChooseConfig);

  GLS_WAIT_SET_RET_PTR(ret, eglChooseConfig);
  *num_config = ret->num_config;

  if (configs) {
    assert(*num_config <= config_size);
    memcpy(configs, ret->configs, ret->num_config * sizeof(EGLConfig));
  }

  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglCopyBuffers,  EGLDisplay dpy, EGLSurface surface, NativePixmapType target )
{
  (void)dpy; (void)surface; (void)target; // FIXME stub
  WARN_STUBBED();
  return EGL_TRUE;
}

GLS_DEF_CORE_API(EGLContext, eglCreateContext,  EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint* attrib_list )
{
  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreateContext);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  c->share_list = (uint64_t)share_list;
  GLS_SEND_PACKET(eglCreateContext);

  GLS_WAIT_SET_RET_PTR(ret, eglCreateContext);
  GLS_RELEASE_RETURN_RET(EGLContext, ret, context);
}

GLS_DEF_CORE_API(EGLSurface, eglCreatePbufferSurface,  EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list )
{
  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreatePbufferSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  GLS_SEND_PACKET(eglCreatePbufferSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglCreatePbufferSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

GLS_DEF_CORE_API(EGLSurface, eglCreatePixmapSurface,  EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint* attrib_list )
{
#if 0
  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreatePixmapSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  //c->pixmap = pixmap; // FIXME must transfer Pixmap first
  GLS_SEND_PACKET(eglCreatePixmapSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglCreatePixmapSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
#else
  (void)dpy; (void)config; (void)pixmap; (void)attrib_list; // FIXME stub
  WARN_STUBBED();
  client_egl_error = EGL_BAD_MATCH; // "config does not support rendering to pixmaps"
  return EGL_NO_SURFACE;
#endif
}

// send information about local window so server can recreate it
static EGLint gls_request_window(NativeWindowType window)
{
#if defined(USE_X11)
  egl_clt_context.x_window = window;
  XWindowAttributes xWindowAttrs;
  if (!XGetWindowAttributes(egl_clt_context.native_display, egl_clt_context.x_window, &xWindowAttrs)) {
    LOGE("XGetWindowAttributes failed\n");
    return EGL_BAD_NATIVE_WINDOW;
  }

  gls_cmd_CREATE_WINDOW(egl_clt_context.x_window, xWindowAttrs.width, xWindowAttrs.height);
#endif
  return EGL_SUCCESS;
}

// near-dup of eglCreatePlatformWindowSurfaceEXT
GLS_DEF_CORE_API(EGLSurface, eglCreateWindowSurface,  EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint* attrib_list )
{
  {
    EGLint err = gls_request_window(window);
    if (err != EGL_SUCCESS) {
      client_egl_error = err;
      return EGL_NO_SURFACE;
    }
  }

  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreateWindowSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  c->window = window;
  GLS_SEND_PACKET(eglCreateWindowSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglCreateWindowSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

GLS_DEF_CORE_API(EGLBoolean, eglDestroyContext,  EGLDisplay dpy, EGLContext ctx )
{
  GLS_SET_COMMAND_PTR(c, eglDestroyContext);
  c->dpy = (uint64_t)dpy;
  c->ctx = (uint64_t)ctx;
  GLS_SEND_PACKET(eglDestroyContext);

  GLS_WAIT_SET_RET_PTR(ret, eglDestroyContext);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglDestroySurface,  EGLDisplay dpy, EGLSurface surface )
{
  GLS_SET_COMMAND_PTR(c, eglDestroySurface);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  GLS_SEND_PACKET(eglDestroySurface);

  GLS_WAIT_SET_RET_PTR(ret, eglDestroySurface);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglGetConfigAttrib,  EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value )
{
  GLS_SET_COMMAND_PTR(c, eglGetConfigAttrib);
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  c->attribute = attribute;
  GLS_SEND_PACKET(eglGetConfigAttrib);

  GLS_WAIT_SET_RET_PTR(ret, eglGetConfigAttrib);
  *value = ret->value;

  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglGetConfigs,  EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config )
{
  GLS_SET_COMMAND_PTR(c, eglGetConfigs);
  c->dpy = (uint64_t)dpy;
  c->config_size = configs ? config_size : 0;
  GLS_SEND_PACKET(eglGetConfigs);

  GLS_WAIT_SET_RET_PTR(ret, eglGetConfigs);
  *num_config = ret->num_config;
  if (configs) {
    assert(*num_config <= config_size);
    memcpy(configs, ret->configs, *num_config * sizeof(EGLConfig));
  }
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLDisplay, eglGetCurrentDisplay, void)
{
  GLS_SET_COMMAND_PTR(c, eglGetCurrentDisplay);
  GLS_SEND_PACKET(eglGetCurrentDisplay);

  GLS_WAIT_SET_RET_PTR(ret, eglGetCurrentDisplay);
  GLS_RELEASE_RETURN_RET(EGLDisplay, ret, display);
}

GLS_DEF_CORE_API(EGLSurface, eglGetCurrentSurface, EGLint readdraw)
{
  GLS_SET_COMMAND_PTR(c, eglGetCurrentSurface);
  c->readdraw = readdraw;
  GLS_SEND_PACKET(eglGetCurrentSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglGetCurrentSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

GLS_DEF_CORE_API(EGLDisplay, eglGetDisplay, NativeDisplayType native_display)
{
  GLS_SET_COMMAND_PTR(c, eglGetDisplay);
  if (gls_wire_native_display(native_display, &c->native_display) < 0)
    return EGL_NO_DISPLAY; // but no error, spec says
  GLS_SEND_PACKET(eglGetDisplay);

  GLS_WAIT_SET_RET_PTR(ret, eglGetDisplay);
  GLS_RELEASE_RETURN_RET(EGLDisplay, ret, display);
}

GLS_DEF_CORE_API(EGLint, eglGetError,  void )
{
  if (client_egl_error != EGL_SUCCESS)
    return client_egl_error;

  GLS_SET_COMMAND_PTR(c, eglGetError);
  GLS_SEND_PACKET(eglGetError);

  GLS_WAIT_SET_RET_PTR(ret, eglGetError);
  GLS_RELEASE_RETURN_RET(EGLint, ret, error);
}

GLS_DEF_CORE_API(__eglMustCastToProperFunctionPointerType, eglGetProcAddress,  const char* procname )
{
  size_t procname_len = strlen(procname);
  if (sizeof(gls_eglGetProcAddress_t) + procname_len + 1 >
      xmitr_getbufsize(glsc_global.api_xmitr)) {
    LOGE("%s: procname '%s' too long for buffer\n",
         __FUNCTION__, procname);
    return NULL;
  }

  GLS_SET_COMMAND_PTR(c, eglGetProcAddress);
  _GLS_VARIABLE_PAYLOAD(c, procname, procname_len, return NULL);
  GLS_SEND_PACKET(eglGetProcAddress);
  GLS_WAIT_SET_RET_PTR(ret, eglGetProcAddress);

  void* proc = NULL;
  if (ret) {
    if (strncmp(procname, "egl", 3) == 0)
      proc = gls_GetEglProcAddress(procname);
    else if (strncmp(procname, "gl", 2) == 0)
      proc = gls_GetGlesProcAddress(procname);
    else
      LOGW("%s: called for valid func of unknown API: '%s'\n",
           __FUNCTION__, procname);
  }
  // else hide the symbol even if GLS supports it

  GLS_RELEASE_RET();
  return proc;
}

GLS_DEF_CORE_API(EGLBoolean, eglInitialize,  EGLDisplay dpy, EGLint* major, EGLint* minor )
{
  GLS_SET_COMMAND_PTR(c, eglInitialize);
  c->dpy = (uint64_t)dpy;
  GLS_SEND_PACKET(eglInitialize);

  GLS_WAIT_SET_RET_PTR(ret, eglInitialize);
  if (major)
    *major = ret->major;
  if (minor)
    *minor = ret->minor;
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglMakeCurrent, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
  GLS_SET_COMMAND_PTR(c, eglMakeCurrent);
  c->dpy = (uint64_t)dpy;
  c->draw = (uint64_t)draw;
  c->read = (uint64_t)read;
  c->ctx = (uint64_t)ctx;
  GLS_SEND_PACKET(eglMakeCurrent);

  GLS_WAIT_SET_RET_PTR(ret, eglMakeCurrent);
  return ret->success;
}

GLS_DEF_CORE_API(EGLBoolean, eglQueryContext,  EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value )
{
  GLS_SET_COMMAND_PTR(c, eglQueryContext);
  c->dpy = (uint64_t)dpy;
  c->ctx = (uint64_t)ctx;
  c->attribute = attribute;
  GLS_SEND_PACKET(eglQueryContext);

  GLS_WAIT_SET_RET_PTR(ret, eglQueryContext);
  *value = ret->value;
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

// eglQueryString (with caching)

static const char* EGLAPIENTRY _real_eglQueryString(EGLDisplay dpy, EGLint name)
{
  GLS_SET_COMMAND_PTR(c, eglQueryString);
  c->dpy = (uint64_t)dpy;
  c->name = name;
  GLS_SEND_PACKET(eglQueryString);

  GLS_WAIT_SET_RET_PTR(ret, eglQueryString);
  if (!ret->success) {
    GLS_RELEASE_RET();
    return NULL;
  }
  // emulating static buffer: must defer freeing until after copied
  return ret->params;
}

static char* eglquerystring_client_extensions;

#define GLS_EGLQUERYSTRING_ITEMS() \
  EMPTY()                          \
    X(CLIENT_APIS)                 \
    X(VENDOR)                      \
    X(VERSION)                     \
    X(EXTENSIONS)                  \
  //

static struct
{
  EGLDisplay dpy;
  char* storage;
  size_t allocated;
  size_t nfilled;

#define X(FIELD) const char* FIELD##_str;
  GLS_EGLQUERYSTRING_ITEMS();
#undef X
} egl_strings;

// record a string into egl_strings
static int _registerEglString(EGLDisplay dpy, EGLint name, const char** field_p)
{
  const char* value = _real_eglQueryString(dpy, name);
  if (!value) {
    EGLint error = eglGetError();
    if (error != EGL_BAD_PARAMETER)
      LOGE("eglQueryString(%p, 0x%x) failed, error 0x%x\n",
           dpy, name, error);
    return 0;
  }
  int valuesize = strlen(value) + 1;
  while (egl_strings.nfilled + valuesize > egl_strings.allocated) {
    egl_strings.allocated *= 2;
    //LOGD("eglQueryString reallocating %zu\n", egl_strings.allocated);
    void* newstorage = realloc(egl_strings.storage, egl_strings.allocated);
    if (!newstorage) {
      LOGE("eglQueryString reallocation failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    egl_strings.storage = newstorage;
  }
  char* field = egl_strings.storage + egl_strings.nfilled;
  strcpy(field, value);
  *field_p = field;
  egl_strings.nfilled += valuesize;
  GLS_RELEASE_RET();
  return 1;
}

static void _populate_egl_strings(EGLDisplay dpy)
{
  assert(!egl_strings.dpy);
  assert(!egl_strings.allocated);
  egl_strings.dpy = dpy;
  egl_strings.allocated = 1024; // rather arbitrary
  egl_strings.storage = malloc(egl_strings.allocated);
  if (!egl_strings.storage) {
    LOGE("eglQueryString allocation failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

#define X(FIELD)                                                        \
  if (!_registerEglString(dpy, EGL_##FIELD, &egl_strings.FIELD##_str))  \
    exit(EXIT_FAILURE);                                                 \
  //

  GLS_EGLQUERYSTRING_ITEMS();
#undef X
}

// FIXME: since we're handling queries mostly locally, we should do
// more checks and fill client_egl_error properly, following spec
GLS_DEF_CORE_API(const char*, eglQueryString, EGLDisplay dpy, EGLint name)
{
  // handle the client-extensions special case
  if (dpy == EGL_NO_DISPLAY) {
    if (name != EGL_EXTENSIONS) {
      client_egl_error = EGL_BAD_DISPLAY;
      return NULL;
    }
    if (!eglquerystring_client_extensions) {
      // query and cache
      const char* value = _real_eglQueryString(dpy, name);
      if (!value) return NULL;
      eglquerystring_client_extensions = malloc(strlen(value) + 1);
      if (!eglquerystring_client_extensions) {
        LOGE("eglQueryString allocation failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      strcpy(eglquerystring_client_extensions, value);
      GLS_RELEASE_RET();
    }
    client_egl_error = EGL_SUCCESS;
    return eglquerystring_client_extensions;
  }

  // init storage
  if (!egl_strings.storage)
    _populate_egl_strings(dpy);

  // check dpy against cache
  if (egl_strings.dpy != dpy) {
    LOGE("eglQueryString() called for a second display, values will be wrong\n");
    exit(EXIT_FAILURE);
  }

  switch (name) {
#define X(FIELD)                                                        \
    case EGL_##FIELD:                                                   \
      if (!egl_strings.FIELD##_str) {                                   \
        client_egl_error = EGL_BAD_PARAMETER;                           \
        return NULL;                                                    \
      }                                                                 \
      client_egl_error = EGL_SUCCESS;                                   \
      return egl_strings.FIELD##_str;                                   \
      //
    GLS_EGLQUERYSTRING_ITEMS();
#undef X

  default:
    client_egl_error = EGL_BAD_PARAMETER;
    return NULL;
  }
}

GLS_DEF_CORE_API(EGLBoolean, eglQuerySurface,  EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value )
{
  GLS_SET_COMMAND_PTR(c, eglQuerySurface);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  c->attribute = attribute;
  GLS_SEND_PACKET(eglQuerySurface);

  GLS_WAIT_SET_RET_PTR(ret, eglQuerySurface);
  *value = ret->value;
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglSwapBuffers,  EGLDisplay dpy, EGLSurface draw )
{
  GLS_SET_COMMAND_PTR(c, eglSwapBuffers);
  c->dpy = (uint64_t)dpy;
  c->draw = (uint64_t)draw;
  GLS_SEND_PACKET(eglSwapBuffers);

  GLS_WAIT_SET_RET_PTR(ret, eglSwapBuffers);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglTerminate,  EGLDisplay dpy )
{
  GLS_SET_COMMAND_PTR(c, eglTerminate);
  c->dpy = (uint64_t)dpy;
  GLS_SEND_PACKET(eglTerminate);

  GLS_WAIT_SET_RET_PTR(ret, eglTerminate);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglWaitGL,  void )
{
  WARN_STUBBED();
  return // EGL_TRUE;
    EGL_FALSE;
}

GLS_DEF_CORE_API(EGLBoolean, eglWaitNative,  EGLint engine )
{
  (void)engine; // FIXME stub
  WARN_STUBBED();
  return EGL_FALSE;
}

// EGL 1.1

GLS_DEF_CORE_API(EGLBoolean, eglBindTexImage, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
  GLS_SET_COMMAND_PTR(c, eglBindTexImage);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  c->buffer = buffer;
  GLS_SEND_PACKET(eglBindTexImage);

  GLS_WAIT_SET_RET_PTR(ret, eglBindTexImage);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglReleaseTexImage,  EGLDisplay dpy, EGLSurface surface, EGLint buffer )
{
  GLS_SET_COMMAND_PTR(c, eglReleaseTexImage);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  c->buffer = buffer;
  GLS_SEND_PACKET(eglReleaseTexImage);

  GLS_WAIT_SET_RET_PTR(ret, eglReleaseTexImage);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglSurfaceAttrib, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
  GLS_SET_COMMAND_PTR(c, eglSurfaceAttrib);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  c->attribute = attribute;
  c->value = value;
  GLS_SEND_PACKET(eglSurfaceAttrib);

  GLS_WAIT_SET_RET_PTR(ret, eglSurfaceAttrib);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglSwapInterval, EGLDisplay dpy, EGLint interval)
{
  GLS_SET_COMMAND_PTR(c, eglSwapInterval);
  c->dpy = (uint64_t)dpy;
  c->interval = interval;
  GLS_SEND_PACKET(eglSwapInterval);

  GLS_WAIT_SET_RET_PTR(ret, eglSwapInterval);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

// EGL 1.2

GLS_DEF_CORE_API(EGLBoolean, eglBindAPI, EGLenum api)
{
  GLS_SET_COMMAND_PTR(c, eglBindAPI);
  c->api = api;
  GLS_SEND_PACKET(eglBindAPI);

  GLS_WAIT_SET_RET_PTR(ret, eglBindAPI);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLenum, eglQueryAPI, void)
{
  WARN_STUBBED();
  return 0;
}

GLS_DEF_CORE_API(EGLSurface, eglCreatePbufferFromClientBuffer, EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list)
{
  (void)dpy; (void)buftype; (void)buffer; (void)config; (void)attrib_list;
  WARN_STUBBED();
  return 0;
}

GLS_DEF_CORE_API(EGLBoolean, eglReleaseThread, void)
{
  GLS_SET_COMMAND_PTR(c, eglReleaseThread);
  GLS_SEND_PACKET(eglReleaseThread);

  GLS_WAIT_SET_RET_PTR(ret, eglReleaseThread);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

GLS_DEF_CORE_API(EGLBoolean, eglWaitClient, void)
{
  WARN_STUBBED();
  return EGL_FALSE;
}


// EGL 1.4

GLS_DEF_CORE_API(EGLContext, eglGetCurrentContext, void)
{
  GLS_SET_COMMAND_PTR(c, eglGetCurrentContext);
  GLS_SEND_PACKET(eglGetCurrentContext);

  GLS_WAIT_SET_RET_PTR(ret, eglGetCurrentContext);
  GLS_RELEASE_RETURN_RET(EGLContext, ret, context);
}


// EGL 1.5

GLS_DEF_CORE_API(EGLSync, eglCreateSync, EGLDisplay dpy, EGLenum type, const EGLAttrib* attrib_list)
{
  (void)dpy; (void)type; (void)attrib_list;
  WARN_STUBBED();
  return  EGL_NO_SYNC;
}

GLS_DEF_CORE_API(EGLBoolean, eglDestroySync, EGLDisplay dpy, EGLSync sync)
{
  (void)dpy; (void)sync;
  WARN_STUBBED();
  return EGL_FALSE;
}

GLS_DEF_CORE_API(EGLint, eglClientWaitSync, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{
  (void)dpy; (void)sync; (void)flags; (void)timeout;
  WARN_STUBBED();
  return EGL_FALSE;
}

GLS_DEF_CORE_API(EGLBoolean, eglGetSyncAttrib, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib* value)
{
  (void)dpy; (void)sync; (void)attribute; (void)value;
  WARN_STUBBED();
  return EGL_FALSE;
}

GLS_DEF_CORE_API(EGLImage, eglCreateImage, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list)
{
  (void)dpy; (void)ctx; (void)target; (void)buffer; (void)attrib_list;
  WARN_STUBBED();
  return EGL_NO_IMAGE;
}

GLS_DEF_CORE_API(EGLBoolean, eglDestroyImage, EGLDisplay dpy, EGLImage image)
{
  (void)dpy; (void)image;
  WARN_STUBBED();
  return EGL_FALSE;
}

// near-dup of eglGetPlatformDisplayEXT
GLS_DEF_CORE_API(EGLDisplay, eglGetPlatformDisplay, EGLenum platform, void* native_display, const EGLAttrib* attrib_list)
{
  WARN_UNTESTED(); // ... though eglGetPlatformDisplayEXT was
  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglGetPlatformDisplay);
  c->platform = platform;
  if (gls_wire_native_display(native_display, &c->native_display) < 0)
    return EGL_NO_DISPLAY; // but no error, spec says
  c->has_attribs = has_attribs;
  GLS_SEND_PACKET(eglGetPlatformDisplay);

  GLS_WAIT_SET_RET_PTR(ret, eglGetPlatformDisplay);
  GLS_RELEASE_RETURN_RET(EGLDisplay, ret, display);
}

GLS_DEF_CORE_API(EGLSurface, eglCreatePlatformWindowSurface, EGLDisplay dpy, EGLConfig config, void* native_window, const EGLAttrib* attrib_list)
{
  (void)dpy; (void)config; (void)native_window; (void)attrib_list;
  WARN_STUBBED();
  return EGL_NO_SURFACE;
}

GLS_DEF_CORE_API(EGLSurface, eglCreatePlatformPixmapSurface, EGLDisplay dpy, EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list)
{
  (void)dpy; (void)config; (void)native_pixmap; (void)attrib_list;
  WARN_STUBBED();
  return EGL_NO_SURFACE;
}

GLS_DEF_CORE_API(EGLBoolean, eglWaitSync, EGLDisplay dpy, EGLSync sync, EGLint flags)
{
  (void)dpy; (void)sync; (void)flags;
  WARN_STUBBED();
  return EGL_FALSE;
}


// Extensions

#ifdef EGL_EXT_platform_base
// near-dup of eglGetPlatformDisplay
static EGLAPI EGLDisplay EGLAPIENTRY __GLS_eglGetPlatformDisplayEXT(EGLenum platform, void* native_display, const EGLint* attrib_list)
{
  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglGetPlatformDisplayEXT);
  c->platform = platform;
  if (gls_wire_native_display(native_display, &c->native_display) < 0)
    return EGL_NO_DISPLAY; // but no error, spec says
  c->has_attribs = has_attribs;
  GLS_SEND_PACKET(eglGetPlatformDisplayEXT);

  GLS_WAIT_SET_RET_PTR(ret, eglGetPlatformDisplayEXT);
  GLS_RELEASE_RETURN_RET(EGLDisplay, ret, display);
}

// near-dup of eglCreateWindowSurface
static EGLAPI EGLSurface EGLAPIENTRY __GLS_eglCreatePlatformWindowSurfaceEXT (EGLDisplay dpy, EGLConfig config, void* native_window, const EGLint* attrib_list)
{
  {
    EGLint err = gls_request_window(*(NativeWindowType*)native_window);
    if (err != EGL_SUCCESS) {
      client_egl_error = err;
      return EGL_NO_SURFACE;
    }
  }

  SEND_ATTRIB_DATA(has_attribs, attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreatePlatformWindowSurfaceEXT);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  c->window = (uint32_t)(uint64_t)native_window;
  assert((uint64_t)native_window == c->window);
  GLS_SEND_PACKET(eglCreatePlatformWindowSurfaceEXT);

  GLS_WAIT_SET_RET_PTR(ret, eglCreatePlatformWindowSurfaceEXT);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

static EGLAPI EGLSurface EGLAPIENTRY __GLS_eglCreatePlatformPixmapSurfaceEXT (EGLDisplay dpy, EGLConfig config, void* native_pixmap, const EGLint* attrib_list)
{
  (void)dpy; (void)config; (void)native_pixmap; (void)attrib_list;
  WARN_STUBBED();
  return EGL_NO_SURFACE;
}
#endif


// eglGetProcAddress support

void* gls_GetEglProcAddress(const char* procname)
{
  void* proc;
  if (0) {}
#define X(FUNC)                            \
  else if (strcmp(procname, #FUNC) == 0) { \
    proc = __GLS_##FUNC;                   \
  }                                        \
  //
  GLS_EGL_COMMANDS()
  GLS_EGL_EXT_COMMANDS()
#undef X
  else {
    LOGW("%s: %s available on server but not supported\n",
         __FUNCTION__, procname);
    proc = NULL;
  }

  return proc;
}
