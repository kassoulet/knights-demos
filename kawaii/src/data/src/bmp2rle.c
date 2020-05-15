/*
 * Intro Data Importer (c) 2004 Gautier Portet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
   8 colors RLE packer
   format: msb is the rle flag, the 3 others bits, the data.

   [7|654] [3|210]
    F nn1   F nn2

   if flag == 0 -> normal pixel, data is the color.
   if flag == 1 -> rle: repeat previous color n+2 times
	WARNING: if the rle data is 0, then we repeat 2 pixels!, 1->3, 2->4, etc...

   many rle format were tested (over 20!) and this one is the best with the upx packing that follow
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

char* files[13] = {
	"font",
	"kitty",
	"babies",
	"bunny",
	"k1",
	"k2",
	"k3",
	"back",
	"greets",
	NULL
};



/* The screen surface */
SDL_Surface *screen = NULL;

int nibble_index = 0;
Uint8 nibble_buffer[150000];

Uint8 nibble_byte = 0;
int nibble_cached = 0;

void WriteNibble(FILE* f, Uint32 nibble)
{
	//nibble_buffer[nibble_index++] = nibble;
	//return;

	nibble <<= 4;
	nibble_byte >>= 4;
	nibble_byte &= nibble | 0x0f;
	nibble_byte |= nibble & 0xf0;

	if (nibble_cached) {
		nibble_buffer[nibble_index++] = nibble_byte;
		nibble_cached = 0;
	} else {
		nibble_cached++;
	}
}

void BeginNibble()
{
	nibble_byte = 0;
	nibble_cached = 0;
	nibble_index = 0;
}

void EndNibble(FILE* f)
{
	int i;

	WriteNibble(f, 0);

	for (i = 0; i < nibble_index; i++) {
		fprintf(f, "%d, ", nibble_buffer[i]);
		if (i % 20 == 19)
			fprintf(f, "\n");
	}
}


/* output one pixel */
void EncodePixel_4(FILE* f, Uint8 color)
{
	WriteNibble(f, color);
}

void EncodeSpan_4b(FILE * f, Uint8 color, int lenght)
{
	int i;

	/*
		for(i=0; i<lenght; i++) {
			WriteNibble(f, color);
		}
		return;
	*/

	lenght -= 1;

	WriteNibble(f, color);

	while (lenght) {
		int l;

		l = lenght - 2;
		if (l > 7) {
			l = 7;
		}

		if (l >= 0) {
			WriteNibble(f, l | 0x08);
		} else {
			WriteNibble(f, color);
		}
		lenght -= l + 2;
	}
}

/* output one pixel */
void EncodePixel(FILE* f, Uint8 color)
{
	EncodePixel_4(f, color);
}

/* output one row of pixel with the same color */
void EncodeSpan(FILE * f, Uint8 color, int lenght)
{
	EncodeSpan_4b(f, color, lenght);
}


/* handle one line */
void EncodeScanline( FILE *f, Uint8* p, int width)
{
	int x;
	int current, old;
	int lenght = 1;
	Uint8* tmp;

	old = -1;

	for (x = 0; x < width; x++) {
		current = *p++;
		if ( (current == old)  ) {
			/* same color, increment counter */
			lenght++;
		} else {
			/* different pixel */
			if (lenght > 1) {
				/* was a span */
				EncodeSpan(f, old, lenght);
			} else {
				/* was a pixel */
				if (x > 0)
					EncodePixel(f, old);
			}
			lenght = 1;
		}
		old = current;
	}
	/* residual pixels */
	if (lenght > 1) {
		EncodeSpan(f, old, lenght);
	} else {
		EncodePixel(f, old);
	}
}

void SaveBitmap(char* filename, char* destination, char* name)
{
	int x, y;
	int nbColors = 0;
	int i;
	int histo[256];
	FILE *f;
	char str[1024];
	int green = 0;

	printf("Processing Bitmap: %s\n", filename);

	/* loading */
	SDL_Surface *bmp;
	bmp = SDL_LoadBMP(filename);
	if (bmp == NULL) {
		char msg[1024];
		sprintf (msg, "Cannot Load %s: %s\n", filename, SDL_GetError ());
		exit (2);
	}

	/* count colors */
	for (i = 0; i < 256; i++) {
		histo[i] = 0;
	}
	for (y = 0; y < bmp->h; y++) {
		Uint8 *p = ((Uint8*)bmp->pixels) + bmp->pitch * y;
		for (x = 0; x < bmp->w; x++) {
			Uint8 c;
			c = *p++;
			histo[c]++;
		}
	}
	for (i = 0; i < 256; i++) {
		if (histo[i]) {
			nbColors++;
		}
	}
	printf("  %d unique colors\n", nbColors);

	/* find and handle the colorkey */
	for (i = 1; i < 255; i++) {
		int r, g, b;
		r = bmp->format->palette->colors[i].r;
		g = bmp->format->palette->colors[i].g;
		b = bmp->format->palette->colors[i].b;
		if ( (r == 0) && (g == 255) && (b == 0) ) {
			green = i;
		}
	}
	if (green) {
		/* force the green to be color 0 */
		printf("**** GREENing %s ****\n", str);
		bmp->format->palette->colors[green].r = bmp->format->palette->colors[0].r;
		bmp->format->palette->colors[green].g = bmp->format->palette->colors[0].g;
		bmp->format->palette->colors[green].b = bmp->format->palette->colors[0].b;
		bmp->format->palette->colors[0].r = 0;
		bmp->format->palette->colors[0].g = 255;
		bmp->format->palette->colors[0].b = 0;

		for (y = 0; y < bmp->h; y++) {
			Uint8 *p = ((Uint8*)bmp->pixels) + bmp->pitch * y;
			for (x = 0; x < bmp->w; x++) {
				Uint8 c;
				c = *p;
				if (c == green) {
					*p = 0;
				}
				if (c == 0) {
					*p = green;
				}
				p++;
			}
		}
		SDL_SaveBMP(bmp, str);
	}


	/* output the file */
	f = fopen(destination, "w");

	fprintf(f,	"/* Generated by SC2 Data Importer */\n");
	fprintf(f,	"#include \"bitmap.h\"\n");

	fprintf(f,	"SCPalette pal_%s[] = {\n", name);
	fprintf(f, "		 %d,%d,%d	\n"
	        , bmp->format->palette->colors[0].b
	        , bmp->format->palette->colors[0].g
	        , bmp->format->palette->colors[0].r);
	y = 1;
	for (i = 1; i < nbColors; i++) {
		/*if (histo[y])*/ {
			fprintf(f, "		,%d,%d,%d	\n",	bmp->format->palette->colors[y].b,
			        bmp->format->palette->colors[y].g,
			        bmp->format->palette->colors[y].r);
		}
		y++;
	}
	fprintf(f, "	};\n");

	fprintf(f,	"SCPixel pix_%s[] = {\n", name);
	BeginNibble();

	for (y = 0; y < bmp->h; y++) {
		Uint8 *p = ((Uint8*)bmp->pixels) + bmp->pitch * y;
		EncodeScanline(f, p, bmp->w);
	}
	EndNibble(f);
	fprintf(f, "0};\n");

	fprintf(f,	"struct RLEBitmap bmp_%s = {\n"
	        "	%d,%d, 	/* widht, height */\n"
	        "	%d,		/* unique colors */\n"
	        "	pal_%s,		\n"
	        "	pix_%s		\n"
	        "};\n"
	        , name, bmp->w, bmp->h, nbColors, name, name);

	fclose(f);
}

int main (int argc, char *argv[])
{
	char msg[1024];
	FILE *f;
	FILE *m;
	unsigned char *buffer;
	int lenght;
	int i;
	char **p;

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		sprintf(msg, "Couldn't initialize SDL: %s\n", SDL_GetError ());
		exit(1);
	}
	atexit(SDL_Quit);

	if (argc < 3) {
		puts("bmp2rle destination source name");
	}


	SaveBitmap(argv[2], argv[1], argv[3]);

	return 0;
}
