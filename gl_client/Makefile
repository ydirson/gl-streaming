WARNINGS = -Wall -Wextra -Werror
OPT = -O3 -g
CFLAGS = $(WARNINGS) $(OPT) -I../common -I. -fPIC

#SANFLAGS = -fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope
CFLAGS += $(SANFLAGS)
LDFLAGS += $(SANFLAGS)
ifeq ($(SANFLAGS),)
LDFLAGS += -Wl,--no-undefined
endif

# Use X11
CFLAGS+=-DUSE_X11

LIBS=-pthread -ldl -lX11
OBJS=fifo.o recvr.o clientegl.o clientgles.o glclient.o

LIBGLES2_2 = libGLESv2.so.2
LIBEGL_1 = libEGL.so.1

LIBGLES2 = libGLESv2.so
LIBEGL = libEGL.so

DESTDIR =
prefix = /usr/local
GLS_LIB_DIR = $(prefix)/lib/gl-streaming

EXES = $(LIBGLES2_2) $(LIBEGL_1) $(LIBGLES2) $(LIBEGL)

all: $(EXES)

fifo.o: ../common/fifo.c ../common/fifo.h
	$(CC) -c -o $@ $(CFLAGS) $<

recvr.o: ../common/recvr.c ../common/recvr.h fifo.o
	$(CC) -c -o $@ $(CFLAGS) $<

clientegl.o: clientegl.c
	$(CC) -c -o $@ $(CFLAGS) $<

clientgles.o: clientgles.c
	$(CC) -c -o $@ $(CFLAGS) $<

glclient.o: glclient.c glclient.h ../common/gls_command.h recvr.o
	$(CC) -c -o $@ $(CFLAGS) $<

$(LIBGLES2_2): $(OBJS)
	$(CC) -shared -o $(LIBGLES2_2) $(CFLAGS) $(OBJS) -Wl,-init,gls_init_library,-fini,gls_cleanup_library $(LDFLAGS) $(LIBS)

$(LIBEGL_1): $(LIBGLES2_2)
	ln -sf $< $@

$(LIBGLES2): $(LIBGLES2_2)
	ln -sf $< $@

$(LIBEGL): $(LIBEGL_1)
	ln -sf $< $@

install:
	mkdir -p $(DESTDIR)$(GLS_LIB_DIR)
	cp $(LIBGLES2_2) $(DESTDIR)$(GLS_LIB_DIR)/
	ln -sf $(LIBGLES2_2) $(DESTDIR)$(GLS_LIB_DIR)/$(LIBEGL_1)

clean:
	rm -f $(EXES) $(OBJS)
