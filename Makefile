CFLAGS=-g `sdl2-config --cflags` `guile-config compile`
LIBS=`sdl2-config --libs` `guile-config link`

scribble: main.o array.o primitive.o guile_scripting.o guile_utils.o effect.o
	g++ $^ -o $@ ${LIBS}

.cpp.o:
	g++ -c $< -o $@ ${CFLAGS}

clean:
	rm -f *.o scribble
