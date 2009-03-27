
CFLAGS = -g -O3 -march=nocona -ffast-math `allegro-config --cflags` `sdl-config --cflags` `libmikmod-config --cflags`
LDLIBS = -g `allegro-config --libs` `sdl-config --libs` `libmikmod-config --libs`


all : styler2

styler2 : styler2.o k3d.o effects.o music.o
	gcc $(LDLIBS) -o styler2 styler2.o k3d.o effects.o music.o

styler2.o: styler2.c

kd3.o: k3d.c

effects.o: effects.c

music.o: music.c

clean :
	rm styler2
	rm *.o

