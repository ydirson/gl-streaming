// This file declare EGL methods for stubs or streaming

#include "gls_command_egl.h"
#include "glclient.h"

#include "EGL/egl.h"

#include <dlfcn.h>
#include <assert.h>
#include <string.h>

#if defined(USE_X11)
Display *xDisplay;
# if !defined(USE_SERVER_SIZE)
Window xWindow;
# endif
#endif

static inline unsigned SEND_ATTRIB_DATA(const EGLint *attrib_list)
{
  if (!attrib_list || attrib_list[0] == EGL_NONE) return 0;
  unsigned num_attribs;
  unsigned data_size;
  for (num_attribs = 0; attrib_list[2*num_attribs] != EGL_NONE; num_attribs++);
  data_size = (num_attribs * 2 + 1) * sizeof(EGLint);
  assert(data_size < GLS_DATA_SIZE * sizeof(EGLint));
  memcpy(glsc_global.pool.tmp_buf.buf, attrib_list, data_size);
  gls_cmd_send_data(data_size, glsc_global.pool.tmp_buf.buf);
  return num_attribs;
}

// EGL 1.0

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig( EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config )
{
    gls_cmd_flush();
    uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
    GLS_SET_COMMAND_PTR(c, eglChooseConfig);
    c->has_attribs = has_attribs;
    c->dpy = (uint64_t)dpy;
    if (configs)
        c->config_size = config_size;
    else
        c->config_size = 0;
    GLS_SEND_PACKET(eglChooseConfig);
    
    wait_for_data("eglChooseConfig");
    gls_ret_eglChooseConfig_t *ret = (gls_ret_eglChooseConfig_t *)glsc_global.pool.tmp_buf.buf;
    *num_config = ret->num_config;
    
    // printf("eglChooseConfig()=%p. config_size=%i,num_config=%i\n", ret->success, config_size, ret->num_config);
    
    if (configs) {
        assert(*num_config <= config_size);
        memcpy(configs, ret->configs, ret->num_config * sizeof(EGLConfig));
    }
    
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers( EGLDisplay dpy, EGLSurface surface, NativePixmapType target )
{
    (void)dpy; (void)surface; (void)target; // FIXME stub
    WARN_STUBBED();
    return EGL_TRUE;
}

EGLAPI EGLContext EGLAPIENTRY eglCreateContext( EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list )
{
    gls_cmd_flush();
    uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
    GLS_SET_COMMAND_PTR(c, eglCreateContext);
    c->has_attribs = has_attribs;
    c->dpy = (uint64_t)dpy;
    c->config = (uint64_t)config;
    c->share_list = (uint64_t)share_list;
    GLS_SEND_PACKET(eglCreateContext);

    wait_for_data("eglCreateContext");
    gls_ret_eglCreateContext_t *ret = (gls_ret_eglCreateContext_t *)glsc_global.pool.tmp_buf.buf;
    return (EGLContext)ret->context;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface( EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list )
{
  gls_cmd_flush();
  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreatePbufferSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  GLS_SEND_PACKET(eglCreatePbufferSurface);

  wait_for_data("eglCreatePbufferSurface");
  gls_ret_eglCreatePbufferSurface_t *ret = (gls_ret_eglCreatePbufferSurface_t *)glsc_global.pool.tmp_buf.buf;
  return (EGLSurface)ret->surface;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface( EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint *attrib_list )
{
#if 0
  gls_cmd_flush();
  uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
  GLS_SET_COMMAND_PTR(c, eglCreatePixmapSurface);
  c->has_attribs = has_attribs;
  c->dpy = (uint64_t)dpy;
  c->config = (uint64_t)config;
  //c->pixmap = pixmap; // FIXME must transfer Pixmap first
  GLS_SEND_PACKET(eglCreatePixmapSurface);

  wait_for_data("eglCreatePixmapSurface");
  gls_ret_eglCreatePixmapSurface_t *ret = (gls_ret_eglCreatePixmapSurface_t *)glsc_global.pool.tmp_buf.buf;
  return (EGLSurface)ret->surface;
#else
    (void)dpy; (void)config; (void)pixmap; (void)attrib_list; // FIXME stub
    WARN_STUBBED();
    client_egl_error = EGL_BAD_MATCH; // "config does not support rendering to pixmaps"
    return EGL_NO_SURFACE;
#endif
}

EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface( EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list )
{
#if defined(USE_X11) && defined(GLS_USE_CLTSIZE)
    if (xWindow && xWindow != window)
        fprintf(stderr, "GLS WARNING: eglCreateWindowSurface: changing X11 Window\n");
    xWindow = window;
#endif

    gls_cmd_flush();
    uint32_t has_attribs = SEND_ATTRIB_DATA(attrib_list);
    GLS_SET_COMMAND_PTR(c, eglCreateWindowSurface);
    c->has_attribs = has_attribs;
    c->dpy = (uint64_t)dpy;
    c->config = (uint64_t)config;
    c->window = 0; // window; FIXME
    GLS_SEND_PACKET(eglCreateWindowSurface);

    wait_for_data("eglCreateWindowSurface");
    gls_ret_eglCreateWindowSurface_t *ret = (gls_ret_eglCreateWindowSurface_t *)glsc_global.pool.tmp_buf.buf;
    return (EGLSurface)ret->surface;
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext( EGLDisplay dpy, EGLContext ctx )
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, eglDestroyContext);
  c->dpy = (uint64_t)dpy;
  c->ctx = (uint64_t)ctx;
  GLS_SEND_PACKET(eglDestroyContext);

  wait_for_data("eglDestroyContext");
  gls_ret_eglDestroyContext_t *ret = (gls_ret_eglDestroyContext_t *)glsc_global.pool.tmp_buf.buf;
  return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface( EGLDisplay dpy, EGLSurface surface )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglDestroySurface);
    c->dpy = (uint64_t)dpy;
    c->surface = (uint64_t)surface;
    GLS_SEND_PACKET(eglDestroySurface);

    wait_for_data("eglDestroySurface");
    gls_ret_eglDestroySurface_t *ret = (gls_ret_eglDestroySurface_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib( EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetConfigAttrib);
    c->dpy = (uint64_t)dpy;
    c->config = (uint64_t)config;
    c->attribute = attribute;
    GLS_SEND_PACKET(eglGetConfigAttrib);
    
    wait_for_data("eglGetConfigAttrib");
    gls_ret_eglGetConfigAttrib_t *ret = (gls_ret_eglGetConfigAttrib_t *)glsc_global.pool.tmp_buf.buf;
    *value = ret->value;
    
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs( EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetConfigs);
    c->dpy = (uint64_t)dpy;
    c->config_size = config_size;
    GLS_SEND_PACKET(eglGetConfigs);
    
    wait_for_data("eglGetConfigs");
    gls_ret_eglGetConfigs_t *ret = (gls_ret_eglGetConfigs_t *)glsc_global.pool.tmp_buf.buf;
    *num_config = ret->num_config;
    if (configs) {
        assert(*num_config <= config_size);
        memcpy(configs, ret->configs, *num_config * sizeof(EGLConfig));
    }
    return ret->success;
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetCurrentDisplay);
    GLS_SEND_PACKET(eglGetCurrentDisplay);
    
    wait_for_data("eglGetCurrentDisplay");
    gls_ret_eglGetCurrentDisplay_t *ret = (gls_ret_eglGetCurrentDisplay_t *)glsc_global.pool.tmp_buf.buf;
    return (EGLDisplay)ret->display;
}

EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetCurrentSurface);
    c->readdraw = readdraw;
    GLS_SEND_PACKET(eglGetCurrentSurface);
    
    wait_for_data("eglGetCurrentSurface");
    gls_ret_eglGetCurrentSurface_t *ret = (gls_ret_eglGetCurrentSurface_t *)glsc_global.pool.tmp_buf.buf;
    return (EGLSurface)ret->surface;
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(NativeDisplayType native_display)
{
#ifdef USE_X11
    if (xDisplay && xDisplay != native_display)
        fprintf(stderr, "GLS WARNING: eglGetDisplay: changing X11 Display\n");
    xDisplay = native_display;
#endif
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetDisplay);
    if (native_display != EGL_DEFAULT_DISPLAY)
        fprintf(stderr, "GLS WARNING: eglGetDisplay: native_display != EGL_DEFAULT_DISPLAY, forcing EGL_DEFAULT_DISPLAY\n");
    c->native_display = (uint64_t)EGL_DEFAULT_DISPLAY;
    GLS_SEND_PACKET(eglGetDisplay);
    
    wait_for_data("eglGetDisplay");
    gls_ret_eglGetDisplay_t *ret = (gls_ret_eglGetDisplay_t *)glsc_global.pool.tmp_buf.buf;
    return (EGLDisplay)ret->display;
}

EGLAPI EGLint EGLAPIENTRY eglGetError( void )
{
    if (client_egl_error != EGL_SUCCESS)
        return client_egl_error;

    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetError);
    GLS_SEND_PACKET(eglGetError);
    
    wait_for_data("eglGetError");
    gls_ret_eglGetError_t *ret = (gls_ret_eglGetError_t *)glsc_global.pool.tmp_buf.buf;
    return ret->error;
}

EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress( const char *procname )
{
    // Do not stream this command
    // FIXME: should query and return NULL when the server does
    return dlsym(dlopen(NULL, RTLD_LOCAL), procname);
}

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize( EGLDisplay dpy, EGLint *major, EGLint *minor )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglInitialize);
    c->dpy = (uint64_t)dpy;
    GLS_SEND_PACKET(eglInitialize);
    
    wait_for_data("eglInitialize");
    gls_ret_eglInitialize_t *ret = (gls_ret_eglInitialize_t *)glsc_global.pool.tmp_buf.buf;
    if (major)
        *major = ret->major;
    if (minor)
        *minor = ret->minor;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, eglMakeCurrent);
  c->dpy = (uint64_t)dpy;
  c->draw = (uint64_t)draw;
  c->read = (uint64_t)read;
  c->ctx = (uint64_t)ctx;
  GLS_SEND_PACKET(eglMakeCurrent);

  wait_for_data("eglMakeCurrent");
  gls_ret_eglMakeCurrent_t *ret = (gls_ret_eglMakeCurrent_t *)glsc_global.pool.tmp_buf.buf;
  return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext( EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglQueryContext);
    c->dpy = (uint64_t)dpy;
    c->ctx = (uint64_t)ctx;
    c->attribute = attribute;
    GLS_SEND_PACKET(eglQueryContext);
    
    wait_for_data("eglQueryContext");
    gls_ret_eglQueryContext_t *ret = (gls_ret_eglQueryContext_t *)glsc_global.pool.tmp_buf.buf;
    *value = ret->value;
    return ret->success;
}

EGLAPI const char* EGLAPIENTRY eglQueryString( EGLDisplay dpy, EGLint name )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglQueryString);
    c->dpy = (uint64_t)dpy;
    c->name = name;
    GLS_SEND_PACKET(eglQueryString);
    
    wait_for_data("eglQueryString");
    gls_ret_eglQueryString_t *ret = (gls_ret_eglQueryString_t *)glsc_global.pool.tmp_buf.buf;
    if (!ret->success)
        return NULL;
    return ret->params;
}

EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface( EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value )
{
#if defined(USE_X11) && defined(GLS_USE_CLTSIZE)
    // Take the size of client-side window instead of server-side surface.
    // Avoids assertion on size-checking (and unwanted scaling if commenting out
    // assertion) eg. in `es2tri`
    if (xDisplay != NULL && xWindow != 0 && (attribute == EGL_WIDTH || attribute == EGL_HEIGHT)) {
        // FIXME this kludge should only be done for the window surface!
        fprintf(stderr, "GLS: eglQuerySurface: querying client window\n");
        XWindowAttributes xWindowAttrs;
        if (!XGetWindowAttributes(xDisplay, xWindow /* XDefaultRootWindow(xDisplay) */, &xWindowAttrs)) {
            printf("Warning: XGetWindowAttributes failed!");
        } else {
            // printf("WIDTH=%d    HEIGHT=%d\n", xWindowAttrs.width, xWindowAttrs.height);
            switch (attribute) {
                case EGL_WIDTH:
                    *value = xWindowAttrs.width;
                    return EGL_TRUE;
                case EGL_HEIGHT:
                    *value = xWindowAttrs.height;
                    return EGL_TRUE;
            }
        }
    }
#endif
    
    fprintf(stderr, "GLS: eglQuerySurface: querying server window\n");
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglQuerySurface);
    c->dpy = (uint64_t)dpy;
    c->surface = (uint64_t)surface;
    c->attribute = attribute;
    GLS_SEND_PACKET(eglQuerySurface);
    
    wait_for_data("eglQuerySurface");
    gls_ret_eglQuerySurface_t *ret = (gls_ret_eglQuerySurface_t *)glsc_global.pool.tmp_buf.buf;
    *value = ret->value;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers( EGLDisplay dpy, EGLSurface draw )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglSwapBuffers);
    c->dpy = (uint64_t)dpy;
    c->draw = (uint64_t)draw;
    GLS_SEND_PACKET(eglSwapBuffers);

    wait_for_data("eglSwapBuffers");
    gls_ret_eglSwapBuffers_t *ret = (gls_ret_eglSwapBuffers_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglTerminate( EGLDisplay dpy )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglTerminate);
    c->dpy = (uint64_t)dpy;
    GLS_SEND_PACKET(eglTerminate);
    
    wait_for_data("eglTerminate");
    gls_ret_eglTerminate_t *ret = (gls_ret_eglTerminate_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
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
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglBindTexImage);
    c->dpy = (uint64_t)dpy;
    c->surface = (uint64_t)surface;
    c->buffer = buffer;
    GLS_SEND_PACKET(eglBindTexImage);
    
    wait_for_data("eglBindTexImage");
    gls_ret_eglBindTexImage_t *ret = (gls_ret_eglBindTexImage_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage( EGLDisplay dpy, EGLSurface surface, EGLint buffer )
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglReleaseTexImage);
    c->dpy = (uint64_t)dpy;
    c->surface = (uint64_t)surface;
    c->buffer = buffer;
    GLS_SEND_PACKET(eglReleaseTexImage);
    
    wait_for_data("eglReleaseTexImage");
    gls_ret_eglReleaseTexImage_t *ret = (gls_ret_eglReleaseTexImage_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglSurfaceAttrib);
    c->dpy = (uint64_t)dpy;
    c->surface = (uint64_t)surface;
    c->attribute = attribute;
    c->value = value;
    GLS_SEND_PACKET(eglSurfaceAttrib);
    
    wait_for_data("eglSurfaceAttrib");
    gls_ret_eglSurfaceAttrib_t *ret = (gls_ret_eglSurfaceAttrib_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, eglSwapInterval);
  c->dpy = (uint64_t)dpy;
  c->interval = interval;
  GLS_SEND_PACKET(eglSwapInterval);

  wait_for_data("eglSwapInterval");
  gls_ret_eglSwapInterval_t *ret = (gls_ret_eglSwapInterval_t *)glsc_global.pool.tmp_buf.buf;
  return ret->success;
}

// EGL 1.2

EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglBindAPI);
    c->api = api;
    GLS_SEND_PACKET(eglBindAPI);
    
    wait_for_data("eglBindAPI");
    gls_ret_eglBindAPI_t *ret = (gls_ret_eglBindAPI_t *)glsc_global.pool.tmp_buf.buf;
    return ret->success;
}

EGLAPI EGLenum EGLAPIENTRY eglQueryAPI (void)
{
    WARN_STUBBED();
    return 0;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer (EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list)
{
    (void)dpy; (void)buftype; (void)buffer; (void)config; (void)attrib_list;
    WARN_STUBBED();
    return 0;
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, eglReleaseThread);
  GLS_SEND_PACKET(eglReleaseThread);

  wait_for_data("eglReleaseThread");
  gls_ret_eglReleaseThread_t *ret = (gls_ret_eglReleaseThread_t *)glsc_global.pool.tmp_buf.buf;
  return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient (void)
{
    WARN_STUBBED();
    return EGL_FALSE;
}


// EGL 1.4

EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, eglGetCurrentContext);
    GLS_SEND_PACKET(eglGetCurrentContext);
    
    wait_for_data("eglGetCurrentContext");
    gls_ret_eglGetCurrentContext_t *ret = (gls_ret_eglGetCurrentContext_t *)glsc_global.pool.tmp_buf.buf;
    return (EGLContext)ret->context;
}
