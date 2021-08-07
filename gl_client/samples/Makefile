CFLAGS=-O3 -Wall -g

INCLUDES=-I.. -I../include -I../../common
LIBS=-lpthread -lm -ldl -lGLESv2 -lEGL
OBJS=glsutil.o
EXES=sample1 sample2

all: $(EXES)

glsutil.o: glsutil.c glsutil.h
	gcc -c -o glsutil.o $(CFLAGS) $(INCLUDES) glsutil.c

sample1: sample1.c glsutil.o
# ../glclient.o ../clientegl.o ../clientgles.o
	gcc -o sample1 $(CFLAGS) $(INCLUDES) sample1.c $(OBJS) $(LIBS)

sample2: sample2.c glsutil.o
# ../glclient.o ../clientegl.o ../clientgles.o
	gcc -o sample2 $(CFLAGS) $(INCLUDES) sample2.c $(OBJS) $(LIBS)

clean:
	rm -f $(EXES) $(OBJS)
