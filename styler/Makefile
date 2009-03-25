
CFLAGS = -g -O3 -march=nocona -ffast-math `allegro-config --cflags` `sdl-config --cflags` `libmikmod-config --cflags`
LDLIBS = -g `allegro-config --libs` `sdl-config --libs` `libmikmod-config --libs` -lz


all : styler

styler : styler.o k3d.o effects.o music.o
	gcc $(LDLIBS) -o styler styler.o k3d.o effects.o music.o

styler.o: styler.c

kd3.o: k3d.c

effects.o: effects.c

music.o: music.c

clean :
	rm styler
	rm *.o

compress : styler
	upx --best styler
