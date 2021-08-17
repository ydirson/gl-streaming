WARNINGS = -Wall -Wextra -Werror
CFLAGS = $(WARNINGS) -O3 -I../common -I. -I./include -fPIC -g

# Use X11
CFLAGS+=-DUSE_X11

# Fake surface size from client-side window
CFLAGS+=-DGLS_USE_CLTSIZE

# FIXME required for now
CFLAGS+=-DGLS_EMULATE_VBO

LIBS=-pthread -ldl -lX11
OBJS=fifo.o server.o clientegl.o clientgles.o glclient.o

LIB_GLES = libGLESv2.so.2
LIB_EGL = libEGL.so.1

DESTDIR =
prefix = /usr/local
GLS_LIB_DIR = $(prefix)/lib/gl-streaming

EXES = $(LIB_GLES) $(LIB_EGL)

all: $(EXES)

fifo.o: ../common/fifo.c ../common/fifo.h
	$(CC) -c -o fifo.o $(CFLAGS) ../common/fifo.c

server.o: ../common/server.c ../common/server.h fifo.o
	$(CC) -c -o server.o $(CFLAGS) ../common/server.c

clientegl.o: clientegl.c include/EGL/egl.h
	$(CC) -c -o clientegl.o $(CFLAGS) clientegl.c

clientgles.o: clientgles.c include/GLES2/gl2.h
	$(CC) -c -o clientgles.o $(CFLAGS) clientgles.c

glclient.o: glclient.c glclient.h ../common/gls_command.h server.o
	$(CC) -c -o glclient.o $(CFLAGS) glclient.c

$(LIB_GLES): $(OBJS)
	$(CC) -shared -o $(LIB_GLES) $(CFLAGS) $(OBJS) -Wl,-init,gls_init_library,-fini,gls_cleanup_library,--no-undefined $(LIBS)

$(LIB_EGL): $(LIB_GLES)
	ln -sf $(LIB_GLES) $(LIB_EGL)

install:
	mkdir -p $(DESTDIR)$(GLS_LIB_DIR)
	cp $(LIB_GLES) $(DESTDIR)$(GLS_LIB_DIR)/
	ln -sf $(LIB_GLES) $(DESTDIR)$(GLS_LIB_DIR)/$(LIB_EGL)

clean:
	rm -f $(EXES) $(OBJS)
