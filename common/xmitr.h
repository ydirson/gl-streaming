#pragma once
#include <stddef.h>

struct xmitr;
struct gls_connection;

struct xmitr* xmitr_init(struct gls_connection*);
void xmitr_free(struct xmitr*);

void* xmitr_getbuf(struct xmitr*);
size_t xmitr_getbufsize(struct xmitr*);
int xmitr_sendbuf(struct xmitr*, size_t);
