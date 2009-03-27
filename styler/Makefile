
CFLAGS = -g -O3 -march=nocona -ffast-math `allegro-config --cflags` `sdl-config --cflags` `libmikmod-config --cflags`
LDLIBS = -g `allegro-config --libs` `sdl-config --libs` `libmikmod-config --libs`


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

release :
	#-upx --best styler.exe *.dll
	-mkdir knights-styler-ng
	-mkdir knights-styler-ng/src
	cp *.asc *.xm *.dat *.exe *.dll file_id.diz *.txt knights-styler-ng
	cp *.c *.h Makefile Makefile.cross.mingw *.txt knights-styler-ng/src
	zip knights-styler-ng.zip knights-styler-ng/* knights-styler-ng/src/*
	
