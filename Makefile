CFLAGS=`sdl2-config --cflags`
LIBS=`sdl2-config --libs`

scribble: main.o
	gcc $< -o $@ ${LIBS}

.c.o:
	gcc -c $< -o $@ ${CFLAGS}
