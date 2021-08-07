CFLAGS=-O3 -Wall -DUSE_X11 -DGLS_SERVER -g -Werror
CFLAGS += -m32

LIBS=-L/opt/vc/lib/ -lX11 -lGLESv2 -lEGL -lpthread -lrt
# -lopenmaxil -lbcm_host -lvcos -lvchiq_arm 
INCLUDES=-I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I../common -I. -I./include
OBJS=fifo.o glcontrol.o server.o serveregl.o servergles.o glserver.o

all: $(OBJS) gl_server

fifo.o: ../common/fifo.c ../common/fifo.h
	$(CC) -c -o fifo.o $(CFLAGS) ../common/fifo.c

glcontrol.o: glcontrol.c glcontrol.h
	$(CC) -c -o glcontrol.o $(CFLAGS) $(INCLUDES) glcontrol.c

server.o: ../common/server.c ../common/server.h fifo.o
	$(CC) -c -o server.o $(CFLAGS) ../common/server.c

serveregl.o: serveregl.c glserver.h ../common/gls_command.h
	$(CC) -c -o serveregl.o $(CFLAGS) $(INCLUDES) serveregl.c

servergles.o: servergles.c glserver.h ../common/gls_command.h
	$(CC) -c -o servergles.o $(CFLAGS) $(INCLUDES) servergles.c

glserver.o: glserver.c glserver.h ../common/gls_command.h glcontrol.o server.o
	$(CC) -c -o glserver.o $(CFLAGS) $(INCLUDES) glserver.c

gl_server: main.c glserver.o
	$(CC) -o gl_server $(CFLAGS) $(INCLUDES) main.c $(OBJS) $(LIBS)

clean:
	rm -f gl_server $(OBJS)
