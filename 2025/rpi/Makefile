LIBS=-lpthread -lpng -lcamera -lcamera-base -levent -levent_pthreads

CCOPT=-std=c99 -D_DEFAULT_SOURCE -g -Wall
CPPOPT=-std=c++17 -D_DEFAULT_SOURCE -g -Wall -I/usr/include/libcamera

all: futbalista

futbalista:	futbalista.c komunikacia.c kamera.c pngwriter.c log.c camera_module.cpp event_loop.cpp
	gcc -c *.c $(CCOPT)
	g++ -c *.cpp $(CPPOPT)
	g++ -o futbalista *.o $(LIBS) $(CCOPT)

clean:
	rm -f futbalista *.o
