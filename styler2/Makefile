
CFLAGS = -O3 -mcpu=pentium -ffast-math -fomit-frame-pointer -funroll-loops `allegro-config --cflags`
LDLIBS = -ljgmod `allegro-config --libs`


all : styler2

styler2 : styler2.o k3d.o effects.o
	gcc $(LDLIBS) -o styler2 styler2.o k3d.o effects.o 
	cp styler2 ..

styler2.o: styler2.c

kd3.o: k3d.c

effects.o: effects.c

clean :
	rm styler2
	rm *.o

compress : styler2
	upx --best styler2
