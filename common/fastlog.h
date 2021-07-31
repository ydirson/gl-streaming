#ifdef __ANDROID__
#include <android/log.h>

#define TAG "GLStreamingNative"
#else
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ANDROID__
  #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
  #define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
  #define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
  #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)
#else
  #define LOGE(...) fprintf(stderr, __VA_ARGS__)
  #define LOGW(...) fprintf(stderr, __VA_ARGS__)
  #define LOGI(...) fprintf(stderr, __VA_ARGS__)
  #define LOGD(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

