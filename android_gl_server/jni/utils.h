#pragma once
#ifdef __ANDROID__
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern jstring jstrclient_addr;
extern ANativeWindow* glsurfaceview_window;

#endif // __ANDROID__
