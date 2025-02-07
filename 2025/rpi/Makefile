LIBS=-lpthread -lpng 

CCOPT=-std=c99 -D_DEFAULT_SOURCE -g

all: futbalista

futbalista:	futbalista.c komunikacia.c kamera.c pngwriter.c log.c
	gcc -o futbalista $^ $(LIBS) $(CCOPT)

clean:
	rm -f futbalista
