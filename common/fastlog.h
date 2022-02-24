#pragma once

#include <stdio.h>

#define LOGE(...) fprintf(stderr, "GLS ERROR: " __VA_ARGS__)
#define LOGW(...) fprintf(stderr, "GLS WARNING: " __VA_ARGS__)
#define LOGI(...) fprintf(stderr, "GLS INFO: " __VA_ARGS__)
#define LOGD(...) fprintf(stderr, "GLS DEBUG: " __VA_ARGS__)
