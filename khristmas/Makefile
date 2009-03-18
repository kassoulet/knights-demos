
CFLAGS=  -O3 -g `sdl-config --cflags` `libmikmod-config --cflags`
LDFLAGS= -g `sdl-config --libs` -lSDL_image `libmikmod-config --libs`

all: khristmas
    
khristmas: khristmas.o
	g++ $(LDFLAGS) -o khristmas khristmas.o

khristmas.asm: khristmas.cpp
	g++ -c -g -Wa,-a,-ad $(CFLAGS) khristmas.cpp > khristmas.asm
	
khristmas.o: khristmas.cpp
	g++ $(CFLAGS) -o khristmas.o -c khristmas.cpp
	
clean:
	rm khristmas.o khristmas data.pak
	
data::
	zip -r data.pak data/*.*
	
dist: all data
	tar c khristmas data.pak plop.txt | bzip2 > khristmas-linux-x86_64.tar.bz2
