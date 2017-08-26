CFLAGS=`sdl2-config --cflags` `guile-config compile`
LIBS=`sdl2-config --libs` `guile-config link`

scribble: main.o scripting.o
	g++ $^ -o $@ ${LIBS}

.c.o:
	g++ -c $< -o $@ ${CFLAGS}

.cpp.o:
	g++ -c $< -o $@ ${CFLAGS}

clean:
	rm -f *.o scribble
