LIBS=-lpthread -lpng -lcairo -lX11
# -levent -levent_pthreads
#-lcamera -lcamera-base 


CCOPT=-std=c99 -D_DEFAULT_SOURCE -g -Wall
CPPOPT=-std=c++17 -D_DEFAULT_SOURCE -g -Wall 
#-I/usr/include/libcamera

all: futbalista

futbalista:	futbalista.c komunikacia.c kamera.c pngwriter.c log.c v4l_module.cpp gui.c
	gcc -c *.c $(CCOPT)
	g++ -c *.cpp $(CPPOPT)
	g++ -o futbalista *.o $(LIBS) $(CCOPT)

clean:
	rm -f futbalista *.o
