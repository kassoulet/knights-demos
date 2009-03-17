
CFLAGS = -O3 -mtune=nocona -ffast-math -fomit-frame-pointer -funroll-loops `sdl-config --cflags` `allegro-config --cflags` `libmikmod-config --cflags` 
LDLIBS = `allegro-config --libs` `libmikmod-config --libs` `sdl-config --libs`


all : mindlink

mindlink : mindlink.o music.o
mindlink.o: mindlink.c
music.o: music.c

clean :
	rm mindlink
	rm mindlink.o

compress : mindlink
	upx --best mindlink
