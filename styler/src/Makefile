
CFLAGS = -O3 -mcpu=pentium -ffast-math -fomit-frame-pointer -funroll-loops `allegro-config --cflags`
LDLIBS = -ljgmod `allegro-config --libs`


all : styler

styler : styler.o k3d.o effects.o
	gcc $(LDLIBS) -o styler styler.o k3d.o effects.o 
	cp styler ..

styler.o: styler.c

kd3.o: k3d.c

effects.o: effects.c

clean :
	rm styler
	rm *.o

compress : styler
	upx --best styler