CFLAGS=`sdl2-config --cflags` `guile-config compile`
LIBS=`sdl2-config --libs` `guile-config link`

scribble: main.o primitives.o scripting.o
	gcc $^ -o $@ ${LIBS}

.c.o:
	gcc -c $< -o $@ ${CFLAGS}

clean:
	rm -f *.o scribble
