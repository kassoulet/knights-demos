
SUBDIRS = src

noinst_LIBRARIES = libdata.a
libdata_a_CFLAGS = -I..

INCLUDES = $(CFLAGS) -I..

libdata_a_SOURCES = music.c bmp_girl.c bmp_font.c bmp_babies.c bmp_back.c bmp_bunny.c bmp_greets.c bmp_kitty.c bmp_k1.c bmp_k2.c bmp_k3.c

EXTRA_DIST = zalza-karate_muffins.xm bmp_font.bmp bmp_babies.bmp bmp_back.bmp bmp_bunny.bmp bmp_greets.bmp bmp_kitty.bmp bmp_k1.bmp bmp_k2.bmp bmp_k3.bmp

music.c: zalza-karate_muffins.xm
	src/bin2c music.c zalza-karate_muffins.xm data_module

bmp_font.c: font.bmp
	src/bmp2rle bmp_font.c font.bmp font
bmp_babies.c:
	src/bmp2rle bmp_babies.c babies.bmp babies
bmp_back.c: back.bmp
	src/bmp2rle bmp_back.c back.bmp back
bmp_bunny.c: bunny.bmp
	src/bmp2rle bmp_bunny.c bunny.bmp bunny
bmp_greets.c: greets.bmp
	src/bmp2rle bmp_greets.c greets.bmp greets
bmp_kitty.c: kitty.bmp
	src/bmp2rle bmp_kitty.c kitty.bmp kitty
bmp_k1.c: k1.bmp
	src/bmp2rle bmp_k1.c k1.bmp k1
bmp_k2.c: k2.bmp
	src/bmp2rle bmp_k2.c k2.bmp k2
bmp_k3.c: k3.bmp
	src/bmp2rle bmp_k3.c k3.bmp k3

bmp_girl.c: girl.bmp
	src/bmp2rle bmp_girl.c k3.bmp girl

