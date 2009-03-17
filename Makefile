#CFLAGS = -O0 -g -march=nocona -ffast-math -fomit-frame-pointer -funroll-loops `allegro-config --cflags`

CFLAGS = -O3 -g -march=nocona `allegro-config --cflags` `sdl-config --cflags`
LDLIBS = -g -lmikmod `allegro-config --libs` `sdl-config --libs`


all : mindlink

mindlink : mindlink.o music.o
mindlink.o: mindlink.c
music.o: music.c

clean :
	rm mindlink
	rm *.o

compress : mindlink
	upx --best mindlink
