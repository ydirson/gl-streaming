WARNINGS = -Wall -Wextra -Werror
CFLAGS = $(WARNINGS) -O3 -DUSE_X11 -DGLS_SERVER -g

LIBS = -lX11 -lGLESv2 -lEGL -lpthread -lrt
INCLUDES = -I../common -I. -I./include
OBJS = main.o fifo.o glcontrol.o recvr.o serveregl.o servergles.o glserver.o

DESTDIR =
prefix = /usr/local
bindir = $(prefix)/bin

all: gl_server

fifo.o: ../common/fifo.c ../common/fifo.h
	$(CC) -c -o $@ $(CFLAGS) ../common/fifo.c

glcontrol.o: glcontrol.c glcontrol.h
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDES) glcontrol.c

recvr.o: ../common/recvr.c ../common/recvr.h fifo.o
	$(CC) -c -o $@ $(CFLAGS) $<

serveregl.o: serveregl.c glserver.h ../common/gls_command.h
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDES) $<

servergles.o: servergles.c glserver.h ../common/gls_command.h
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDES) $<

glserver.o: glserver.c glserver.h ../common/gls_command.h glcontrol.o recvr.o
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDES) $<

main.o: main.c
	$(CC) -c -o $@ $(CFLAGS) $(INCLUDES) $<

gl_server: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

install:
	mkdir -p $(DESTDIR)$(bindir)
	cp gl_server $(DESTDIR)$(bindir)/

clean:
	rm -f gl_server $(OBJS)
