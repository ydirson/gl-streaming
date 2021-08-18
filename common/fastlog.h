#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#define LOGW(...) fprintf(stderr, __VA_ARGS__)
#define LOGI(...) fprintf(stderr, __VA_ARGS__)
#define LOGD(...) fprintf(stderr, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
