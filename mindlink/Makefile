#CFLAGS = -O0 -g -march=nocona -ffast-math -fomit-frame-pointer -funroll-loops `allegro-config --cflags`

CFLAGS = -O3 -g -march=nocona `allegro-config --cflags` `sdl-config --cflags` `libmikmod-config --cflags`
LDLIBS = -g `allegro-config --libs` `sdl-config --libs` `libmikmod-config --libs` -lz


all : mindlink

mindlink : mindlink.o music.o
mindlink.o: mindlink.c
music.o: music.c

clean :
	rm mindlink
	rm *.o

compress : mindlink
	upx --best mindlink
	
dist : mindlink
	tar c mindlink mindlink.xm mindlink.dat *.asc | bzip2 > mindlink-linux-x86_64.tar.bz2
