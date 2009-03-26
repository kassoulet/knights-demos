
CC = gcc

CFLAGS = -g -O3 -march=nocona -ffast-math `allegro-config --cflags` `sdl-config --cflags` `libmikmod-config --cflags`
LDLIBS = -g `allegro-config --libs` `sdl-config --libs` `libmikmod-config --libs`


all : styler

styler : styler.o k3d.o effects.o music.o
	$(CC) $(LDLIBS) -o styler styler.o k3d.o effects.o music.o

styler.o: styler.c
	$(CC) $(CFLAGS) -o styler.o styler.c
kd3.o: k3d.c
	$(CC) $(CFLAGS) -o k3d.o k3d.c

effects.o: effects.c
	$(CC) $(CFLAGS) -o effects.o effects.c

music.o: music.c
	$(CC) $(CFLAGS) -o music.o music.c

clean :
	rm styler
	rm *.o

compress :
	upx --best styler.exe *.dll
	
release :: compress
	zip *.c *.h *.asc *.xm *.dat *.exe *.dll file_id.diz Makefile* *.txt	
	
