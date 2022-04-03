#pragma once
#include <stddef.h>

struct xmitr;

struct xmitr* xmitr_init(void);
void xmitr_free(struct xmitr*);

void* xmitr_getbuf(struct xmitr*);
size_t xmitr_getbufsize(struct xmitr*);
