// This file declare EGL methods for stubs or streaming

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

#if defined(USE_X11)
Display* xDisplay;
Window xWindow;
#endif

static inline unsigned SEND_ATTRIB_DATA(const EGLint* attrib_list)
{
  if (!attrib_list || attrib_list[0] == EGL_NONE) return 0;
  unsigned num_attribs;
  unsigned data_size;
  for (num_attribs = 0; attrib_list[2 * num_attribs] != EGL_NONE; num_attribs++);
  data_size = (num_attribs * 2 + 1) * sizeof(EGLint);
  assert(data_size < GLS_DATA_SIZE * sizeof(EGLint));
  memcpy(glsc_global.pool.tmp_buf.buf, attrib_list, data_size);
  gls_cmd_send_data(data_size, glsc_global.pool.tmp_buf.buf);
  return num_attribs;
}

// EGL 1.0

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig( EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config )
{
  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
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

EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers( EGLDisplay dpy, EGLSurface surface, NativePixmapType target )
{
  (void)dpy; (void)surface; (void)target; // FIXME stub
  WARN_STUBBED();
  return EGL_TRUE;
}

EGLAPI EGLContext EGLAPIENTRY eglCreateContext( EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint* attrib_list )
{
  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreateContext);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  c->share_list = (uint64_t)share_list;
  GLS_SEND_PACKET(eglCreateContext);

  GLS_WAIT_SET_RET_PTR(ret, eglCreateContext);
  GLS_RELEASE_RETURN_RET(EGLContext, ret, context);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface( EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list )
{
  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreatePbufferSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  GLS_SEND_PACKET(eglCreatePbufferSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglCreatePbufferSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface( EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint* attrib_list )
{
#if 0
  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
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

EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface( EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint* attrib_list )
{
  // send information about local window so server can recreate it
#if defined(USE_X11)
  if (xWindow && xWindow != window) {
    fprintf(stderr, "GLS ERROR: %s: supports only one X11 Window\n", __FUNCTION__);
    client_egl_error = EGL_BAD_NATIVE_WINDOW;
    return EGL_NO_SURFACE;
  }

  xWindow = window;
  XWindowAttributes xWindowAttrs;
  if (!XGetWindowAttributes(xDisplay, xWindow, &xWindowAttrs)) {
    fprintf(stderr, "GLS ERROR: XGetWindowAttributes failed\n");
    client_egl_error = EGL_BAD_NATIVE_WINDOW;
    return EGL_NO_SURFACE;
  }

  gls_cmd_CREATE_WINDOW(xWindowAttrs.width, xWindowAttrs.height);
#endif

  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreateWindowSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  c->window = 0; // window; FIXME
  GLS_SEND_PACKET(eglCreateWindowSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglCreateWindowSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext( EGLDisplay dpy, EGLContext ctx )
{
  GLS_SET_COMMAND_PTR(c, eglDestroyContext);
  c->dpy = (uint64_t)dpy;
  c->ctx = (uint64_t)ctx;
  GLS_SEND_PACKET(eglDestroyContext);

  GLS_WAIT_SET_RET_PTR(ret, eglDestroyContext);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface( EGLDisplay dpy, EGLSurface surface )
{
  GLS_SET_COMMAND_PTR(c, eglDestroySurface);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  GLS_SEND_PACKET(eglDestroySurface);

  GLS_WAIT_SET_RET_PTR(ret, eglDestroySurface);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib( EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value )
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

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs( EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config )
{
  GLS_SET_COMMAND_PTR(c, eglGetConfigs);
  c->dpy = (uint64_t)dpy;
  c->config_size = config_size;
  GLS_SEND_PACKET(eglGetConfigs);

  GLS_WAIT_SET_RET_PTR(ret, eglGetConfigs);
  *num_config = ret->num_config;
  if (configs) {
    assert(*num_config <= config_size);
    memcpy(configs, ret->configs, *num_config * sizeof(EGLConfig));
  }
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
  GLS_SET_COMMAND_PTR(c, eglGetCurrentDisplay);
  GLS_SEND_PACKET(eglGetCurrentDisplay);

  GLS_WAIT_SET_RET_PTR(ret, eglGetCurrentDisplay);
  GLS_RELEASE_RETURN_RET(EGLDisplay, ret, display);
}

EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
  GLS_SET_COMMAND_PTR(c, eglGetCurrentSurface);
  c->readdraw = readdraw;
  GLS_SEND_PACKET(eglGetCurrentSurface);

  GLS_WAIT_SET_RET_PTR(ret, eglGetCurrentSurface);
  GLS_RELEASE_RETURN_RET(EGLSurface, ret, surface);
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(NativeDisplayType native_display)
{
#ifdef USE_X11
  if (xDisplay && xDisplay != native_display)
    fprintf(stderr, "GLS WARNING: eglGetDisplay: changing X11 Display\n");
  xDisplay = native_display;
#endif
  GLS_SET_COMMAND_PTR(c, eglGetDisplay);
  if (native_display != EGL_DEFAULT_DISPLAY)
    fprintf(stderr, "GLS WARNING: eglGetDisplay: native_display != EGL_DEFAULT_DISPLAY, forcing EGL_DEFAULT_DISPLAY\n");
  c->native_display = (uint64_t)EGL_DEFAULT_DISPLAY;
  GLS_SEND_PACKET(eglGetDisplay);

  GLS_WAIT_SET_RET_PTR(ret, eglGetDisplay);
  GLS_RELEASE_RETURN_RET(EGLDisplay, ret, display);
}

EGLAPI EGLint EGLAPIENTRY eglGetError( void )
{
  if (client_egl_error != EGL_SUCCESS)
    return client_egl_error;

  GLS_SET_COMMAND_PTR(c, eglGetError);
  GLS_SEND_PACKET(eglGetError);

  GLS_WAIT_SET_RET_PTR(ret, eglGetError);
  GLS_RELEASE_RETURN_RET(EGLint, ret, error);
}

EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress( const char* procname )
{
  // Do not stream this command
  // FIXME: should query and return NULL when the server does
  void* proc = dlsym(NULL, procname);
  return proc;
}

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize( EGLDisplay dpy, EGLint* major, EGLint* minor )
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

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
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

EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext( EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value )
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
      fprintf(stderr, "GLS ERROR: eglQueryString(%p, 0x%x) failed, error 0x%x\n",
              dpy, name, error);
    return 0;
  }
  int valuesize = strlen(value) + 1;
  while (egl_strings.nfilled + valuesize > egl_strings.allocated) {
    egl_strings.allocated *= 2;
    //fprintf(stderr, "GLS DBG: eglQueryString reallocating %zu\n", egl_strings.allocated);
    void* newstorage = realloc(egl_strings.storage, egl_strings.allocated);
    if (!newstorage) {
      fprintf(stderr, "GLS ERROR: eglQueryString reallocation failed: %s\n", strerror(errno));
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
    fprintf(stderr, "GLS ERROR: eglQueryString allocation failed: %s\n", strerror(errno));
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
EGLAPI const char* EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
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
        fprintf(stderr, "GLS ERROR: eglQueryString allocation failed: %s\n", strerror(errno));
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
    fprintf(stderr, "GLS ERROR: eglQueryString() called for a second display, values will be wrong\n");
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

EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface( EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value )
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

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers( EGLDisplay dpy, EGLSurface draw )
{
  GLS_SET_COMMAND_PTR(c, eglSwapBuffers);
  c->dpy = (uint64_t)dpy;
  c->draw = (uint64_t)draw;
  GLS_SEND_PACKET(eglSwapBuffers);

  GLS_WAIT_SET_RET_PTR(ret, eglSwapBuffers);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglTerminate( EGLDisplay dpy )
{
  GLS_SET_COMMAND_PTR(c, eglTerminate);
  c->dpy = (uint64_t)dpy;
  GLS_SEND_PACKET(eglTerminate);

  GLS_WAIT_SET_RET_PTR(ret, eglTerminate);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL( void )
{
  WARN_STUBBED();
  return // EGL_TRUE;
    EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitNative( EGLint engine )
{
  (void)engine; // FIXME stub
  WARN_STUBBED();
  return EGL_FALSE;
}

// EGL 1.1

EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
  GLS_SET_COMMAND_PTR(c, eglBindTexImage);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  c->buffer = buffer;
  GLS_SEND_PACKET(eglBindTexImage);

  GLS_WAIT_SET_RET_PTR(ret, eglBindTexImage);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage( EGLDisplay dpy, EGLSurface surface, EGLint buffer )
{
  GLS_SET_COMMAND_PTR(c, eglReleaseTexImage);
  c->dpy = (uint64_t)dpy;
  c->surface = (uint64_t)surface;
  c->buffer = buffer;
  GLS_SEND_PACKET(eglReleaseTexImage);

  GLS_WAIT_SET_RET_PTR(ret, eglReleaseTexImage);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
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

EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
  GLS_SET_COMMAND_PTR(c, eglSwapInterval);
  c->dpy = (uint64_t)dpy;
  c->interval = interval;
  GLS_SEND_PACKET(eglSwapInterval);

  GLS_WAIT_SET_RET_PTR(ret, eglSwapInterval);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

// EGL 1.2

EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
  GLS_SET_COMMAND_PTR(c, eglBindAPI);
  c->api = api;
  GLS_SEND_PACKET(eglBindAPI);

  GLS_WAIT_SET_RET_PTR(ret, eglBindAPI);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLenum EGLAPIENTRY eglQueryAPI (void)
{
  WARN_STUBBED();
  return 0;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer (EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list)
{
  (void)dpy; (void)buftype; (void)buffer; (void)config; (void)attrib_list;
  WARN_STUBBED();
  return 0;
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
  GLS_SET_COMMAND_PTR(c, eglReleaseThread);
  GLS_SEND_PACKET(eglReleaseThread);

  GLS_WAIT_SET_RET_PTR(ret, eglReleaseThread);
  GLS_RELEASE_RETURN_RET(EGLBoolean, ret, success);
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient (void)
{
  WARN_STUBBED();
  return EGL_FALSE;
}


// EGL 1.4

EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
  GLS_SET_COMMAND_PTR(c, eglGetCurrentContext);
  GLS_SEND_PACKET(eglGetCurrentContext);

  GLS_WAIT_SET_RET_PTR(ret, eglGetCurrentContext);
  GLS_RELEASE_RETURN_RET(EGLContext, ret, context);
}
