// This file declare EGL methods for stubs or streaming

#include <dlfcn.h>

#include "glclient.h"

EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
    gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglBindAPI);
	c->api = api;
	GLS_SEND_PACKET(eglBindAPI);
    
	wait_for_data("timeout:eglBindAPI");
	gls_ret_eglBindAPI_t *ret = (gls_ret_eglBindAPI_t *)glsc_global.tmp_buf.buf;
	return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib( EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value )
{
#ifdef USE_X11
	if (xDisplay != NULL && attribute == EGL_NATIVE_VISUAL_ID) {
		*value = XVisualIDFromVisual(XDefaultVisual(xDisplay, xScreenId));
		return EGL_TRUE;
	}
#endif
	
	for (int i = 0; i < client_config_size; i+=2) {
		if (client_config_keys[i] == attribute) {
			*value = client_config_values[i];
			return EGL_TRUE;
		}
	}

/*
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetConfigAttrib);
	c->dpy = dpy;
	c->config = config;
	c->attribute = attribute;
	GLS_SEND_PACKET(eglGetConfigAttrib);
    
	wait_for_data("timeout:eglGetConfigAttrib");
	gls_ret_eglGetConfigAttrib_t *ret = (gls_ret_eglGetConfigAttrib_t *)glsc_global.tmp_buf.buf;
	*value = value;
	
	return ret->success;
*/
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs( EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config )
{
/*
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetConfigs);
	c->dpy = dpy;
	c->config_size = config_size;
	GLS_SEND_PACKET(eglGetConfigs);
    
	wait_for_data("timeout:eglGetConfigs");
	gls_ret_eglGetConfigs_t *ret = (gls_ret_eglGetConfigs_t *)glsc_global.tmp_buf.buf;
	*num_config = ret->num_config;
	configs = ret->configs;
	return ret->success;
*/

	// Build a fake local config
	if (client_config_size > 0) {
		*num_config = client_config_size / 2;
		for (int i = 0; i < client_config_size; i+=2) {
			configs[i] = &client_config_values[i+1];
		}
	}
	
	return EGL_TRUE;
}

EGLAPI EGLint EGLAPIENTRY eglGetError( void )
{
	if (client_egl_error != EGL_SUCCESS) {
		EGLint error = client_egl_error;
		client_egl_error = EGL_SUCCESS;
		return error;
	}
	
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetError);
	GLS_SEND_PACKET(eglGetError);
    
	wait_for_data("timeout:eglGetError");
	gls_ret_eglGetError_t *ret = (gls_ret_eglGetError_t *)glsc_global.tmp_buf.buf;
	return ret->error;
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(NativeDisplayType display)
{
/*
    gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetDisplay);
	c->display = display;
	GLS_SEND_PACKET(eglGetDisplay);
    
	wait_for_data("timeout:eglGetDisplay");
	gls_ret_eglGetDisplay_t *ret = (gls_ret_eglGetDisplay_t *)glsc_global.tmp_buf.buf;
	return ret->display;
*/

#ifdef USE_X11
	xDisplay = display;
	xScreenId = DefaultScreen(&xDisplay);
#endif // USE_X11
	
	// Can't getting Android EGL display from Linux Native display?
	return eglGetCurrentDisplay();
}

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize( EGLDisplay dpy, EGLint *major, EGLint *minor )
{
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglInitialize);
	c->dpy = dpy;
	c->major = major;
	c->minor = minor;
	GLS_SEND_PACKET(eglInitialize);
    
	wait_for_data("timeout:eglInitialize");
	gls_ret_eglInitialize_t *ret = (gls_ret_eglInitialize_t *)glsc_global.tmp_buf.buf;
	return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglTerminate( EGLDisplay dpy )
{
    gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglTerminate);
	c->dpy = dpy;
	GLS_SEND_PACKET(eglTerminate);
    
	wait_for_data("timeout:eglTerminate");
	gls_ret_eglTerminate_t *ret = (gls_ret_eglTerminate_t *)glsc_global.tmp_buf.buf;
	return ret->success;
}


EGLAPI const char* EGLAPIENTRY eglQueryString( EGLDisplay dpy, EGLint name )
{
/*
    switch (name) {
		case EGL_VENDOR: return "gl-streaming wrapper";
		case EGL_VERSION: return "1.4";
		default: return "";
	}
*/
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglQueryString);
	c->dpy = dpy;
	c->name = name;
	GLS_SEND_PACKET(eglQueryString);
    
	wait_for_data("timeout:eglQueryString");
	gls_ret_eglQueryString_t *ret = (gls_ret_eglQueryString_t *)glsc_global.tmp_buf.buf;
	return &ret->params[0];
}


EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress( const char *procname )
{
	// Do not implement streaming
    return dlsym(dlopen(NULL, RTLD_LOCAL), procname);
}

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig( EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config )
{
/*
	gls_cmd_flush();
	gls_data_egl_attriblist_t *dat = (gls_data_egl_attriblist_t *)glsc_global.tmp_buf.buf;
	memcpy(dat->attrib_list, attrib_list, GLS_DATA_SIZE);
	gls_cmd_send_data(0, GLS_STRING_SIZE_PLUS, glsc_global.tmp_buf.buf);
	
	GLS_SET_COMMAND_PTR(c, eglChooseConfig);
	c->dpy = dpy;
	c->config_size = config_size;
	GLS_SEND_PACKET(eglChooseConfig);
    
	wait_for_data("timeout:eglChooseConfig");
	gls_ret_eglChooseConfig_t *ret = (gls_ret_eglChooseConfig_t *)glsc_global.tmp_buf.buf;
	*num_config = ret->num_config;
	
	// printf("eglChooseConfig()=%p. config_size=%i,num_config=%i\n", ret->success, config_size, ret->num_config);
	
	if (configs != NULL) {
		*configs = ret->configs;
	}
	
	return ret->success;
*/

	// Build a fake local config
	size_t config_arr_size = sizeof(attrib_list)/sizeof(attrib_list[0]);
	if (config_arr_size > 0) {
		client_config_size = config_arr_size;
		// Skip the last one if it is EGL_NONE
		if (attrib_list[client_config_size - 1] == EGL_NONE) {
			client_config_size -= 1;
		}
		
		// printf("client_config_size=%i\n", client_config_size);
		*num_config = client_config_size / 2;
		for (int i = 0; i < client_config_size; i+=2) {
			if (configs != NULL) configs[i] = &attrib_list[i];
			client_config_keys[i] = attrib_list[i];
			client_config_values[i] = attrib_list[i+1];
		}
	}
	
	return EGL_TRUE;
	
	// return eglGetConfigs(dpy, configs, config_size, num_config);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface( EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list )
{
	if (sizeof(void*) != sizeof(window)) {
		client_egl_error = EGL_BAD_NATIVE_WINDOW;
		return EGL_NO_SURFACE;
	}
#if !defined(GLS_USE_SRVSIZE) && defined(USE_X11)
	xWindow = window;
#endif // GLS_USE_SRVSIZE
	
	return eglGetCurrentSurface(EGL_DRAW);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface( EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint *attrib_list )
{
	return eglGetCurrentSurface(EGL_DRAW);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface( EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list )
{
	return eglGetCurrentSurface(EGL_DRAW);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface( EGLDisplay dpy, EGLSurface surface )
{
    return EGL_TRUE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface( EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value )
{
	// This fix size assert in `es2gears` and `es2tri`.
#if !defined(GLS_USE_SRVSIZE) && defined(USE_X11)
	if (xDisplay != NULL && xWindow != NULL && (attribute == EGL_WIDTH || attribute == EGL_HEIGHT)) {
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
		
		// *value = 300;
		// return EGL_TRUE;
	}
#endif // ndef GLS_USE_SRVSIZE && def USE_X11
	
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglQuerySurface);
	c->dpy = dpy;
	c->surface = surface;
	c->attribute = attribute;
	GLS_SEND_PACKET(eglQuerySurface);
    
	wait_for_data("timeout:eglQuerySurface");
	gls_ret_eglQuerySurface_t *ret = (gls_ret_eglQuerySurface_t *)glsc_global.tmp_buf.buf;
	*value = ret->value;
	return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
	// Current stub
	return EGL_TRUE;
}

/* EGL 1.1 render-to-texture APIs */
EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglSurfaceAttrib);
	c->dpy = dpy;
	c->surface = surface;
	c->attribute = attribute;
	c->value = value;
	GLS_SEND_PACKET(eglSurfaceAttrib);
    
	wait_for_data("timeout:eglSurfaceAttrib");
	gls_ret_eglSurfaceAttrib_t *ret = (gls_ret_eglSurfaceAttrib_t *)glsc_global.tmp_buf.buf;
	return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglBindTexImage);
	c->dpy = dpy;
	c->surface = surface;
	c->buffer = buffer;
	GLS_SEND_PACKET(eglBindTexImage);
    
	wait_for_data("timeout:eglBindTexImage");
	gls_ret_eglBindTexImage_t *ret = (gls_ret_eglBindTexImage_t *)glsc_global.tmp_buf.buf;
	return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage( EGLDisplay dpy, EGLSurface surface, EGLint buffer )
{
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglReleaseTexImage);
	c->dpy = dpy;
	c->surface = surface;
	c->buffer = buffer;
	GLS_SEND_PACKET(eglReleaseTexImage);
    
	wait_for_data("timeout:eglReleaseTexImage");
	gls_ret_eglReleaseTexImage_t *ret = (gls_ret_eglReleaseTexImage_t *)glsc_global.tmp_buf.buf;
	return ret->success;
}

/* EGL 1.1 swap control API */
EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval( EGLDisplay dpy, EGLint interval )
{

}

EGLAPI EGLContext EGLAPIENTRY eglCreateContext( EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list )
{
    // return 1;
	
	// Stub: current
	return eglGetCurrentContext();
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext( EGLDisplay dpy, EGLContext ctx )
{
    return EGL_TRUE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent( EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx )
{
    return EGL_TRUE;
}

EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
    gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetCurrentContext);
	GLS_SEND_PACKET(eglGetCurrentContext);
    
	wait_for_data("timeout:eglGetCurrentContext");
	gls_ret_eglGetCurrentContext_t *ret = (gls_ret_eglGetCurrentContext_t *)glsc_global.tmp_buf.buf;
	return ret->context;
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
    gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetCurrentDisplay);
	GLS_SEND_PACKET(eglGetCurrentDisplay);
    
	wait_for_data("timeout:eglGetCurrentDisplay");
	gls_ret_eglGetCurrentDisplay_t *ret = (gls_ret_eglGetCurrentDisplay_t *)glsc_global.tmp_buf.buf;
	return ret->display;
}

EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
    gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglGetCurrentSurface);
	c->readdraw = readdraw;
	GLS_SEND_PACKET(eglGetCurrentSurface);
    
	wait_for_data("timeout:eglGetCurrentSurface");
	gls_ret_eglGetCurrentSurface_t *ret = (gls_ret_eglGetCurrentSurface_t *)glsc_global.tmp_buf.buf;
	return ret->surface;
}

EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext( EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value )
{
	gls_cmd_flush();
	GLS_SET_COMMAND_PTR(c, eglQueryContext);
	c->dpy = dpy;
	c->ctx = ctx;
	c->attribute = attribute;
	GLS_SEND_PACKET(eglQueryContext);
    
	wait_for_data("timeout:eglQueryContext");
	gls_ret_eglQueryContext_t *ret = (gls_ret_eglQueryContext_t *)glsc_global.tmp_buf.buf;
	*value = ret->value;
	return ret->success;
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL( void )
{
    return // EGL_TRUE;
	EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitNative( EGLint engine )
{
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers( EGLDisplay dpy, EGLSurface draw )
{
    static int frame;
    return gls_cmd_flip(frame++);
}

EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers( EGLDisplay dpy, EGLSurface surface, NativePixmapType target )
{
    return EGL_TRUE;
}

