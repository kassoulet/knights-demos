/* Khristmas - (c) 2000-2007 Gautier Portet < kassoulet  gmail com >
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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


#include <math.h>
#include <stdlib.h> 

#include "SDL.h"
#include "SDL_image.h"
#include "mikmod.h"
#include "zlib.h"

const int ScreenWidth = 640;
const int ScreenHeight = 480;

const int BLUR_STEPS = 32;

SDL_Surface *buffer[2];
SDL_Surface *screen;

float frame=0;
float fps=0;
float delta=0;
Uint32 frame_count=0;
Uint32 diff=0;
			
#define FREQ 44100
#define BUFFERSIZE 2048
#define SAMPLESIZE 16
#define INTERPOLATION 1
MODULE *module;

Uint32 MusicUpdate(Uint32 interval, void *param)
{
	MikMod_Update();
	return 1;
}

void OpenMusic(char* filename)
{
	md_mixfreq = FREQ;
	md_device   = 0;
	md_volume   = 128;
	md_musicvolume = 128;
	md_sndfxvolume = 128;
	md_pansep   = 128;
	md_reverb   = 0;

	MikMod_RegisterAllLoaders();
	MikMod_RegisterAllDrivers();

    if (MikMod_Init("")) {
        fprintf(stderr, "Could not initialize sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return;
    }
    
	module = Player_Load(filename, 32, 0);
	Player_Start(module);
	
	SDL_AddTimer(100, MusicUpdate, 0);
	
	if (!module) {
		fputs("error loading module!",stderr);
		exit(1);
		return;
	}
}


void CloseMusic()
{
	Player_Stop();
	MikMod_Exit();
}


char *greets[32] = {
	"Orange Juice",
	"Gods",
	"Mandarine",
	"Orion",
	"Phelios",
	"Popsy  Team",
	"#demofr",
	"#codefr",
	"#traxfr",
	"#pixelfr",
	"Bomb",
	"Calodox",
	"Condense",
	"Eclipse",
	"Fool",
	"Revelation",
	"Glou",
	"Antishock",
	"Oror",
	NULL
};

#define MAX_SCROLL 6
char scroll[][32] = {
	"Here we go...",
	" ",
	" ",
	" ",
	"---",
	" ",
	" ",
	"Crashing",
	"* Kassoulet *",
	" ",
	"ToShoPinG",
	"* MarbleMad *",
	" ",
	"Nenetting",
	"* Stuff *",
	" ",
	"Remixing",
	"* LogicDream *",
	" ",
	" ",
	"---",
	" ",
	" ",
	"We wish you",
	"a",
	"merry",
	"Khristmas",
	" ",
	" ",
	" ",
	" ",
	"Kassoulet:",
	" ",
	"Delphine",
	"Agathe",
	"Milla",
	"Marble",
	"Stuff",
	"Dreamou",
	"Kaktux",
	"Tigrou",
	"Webep",
	"Eclipse",
	"Bul",
	"#demofr",
	"#codefr",
	"lordk",
	"ak",
	"tuo",
	"keops",
	"odin",
	"patrox",
	"stv",
	"cleaner",
	"splif",
	"klang",
	"willbe",
	"senser",
	"starman",
	"jupi",
	"stil",
	" ",
	"zut, c long",
	"bon, les zotres",
	"nommez-vous",
	"tous seuls",
	" ",
	"Cedric",
	"&",
	"Sylvie",
	" ",
	"les autres :)",
	" ",
	" ",
	"MarbleMad:",
	" ",
	" ",
	"kelly",
	"mumu",
	"phenix",
	"bluebear",
	"patrox",
	"hackstaff",
	"grozours",
	" ",
	" ",
	"LogicDream:",
	" ",
	" ",
	"traven",
	"willbe",
	"med",
	"redribbon",
	"mac mahon",
	"calim",
	"klang",
	"niakool",
	"steve/popsy",
	"senser",
	"patrox",
	"chewbacca",
	"elendil",
	"cyborg jeff",
	"thanerd",
	"knights",
	"#demofr",
	" ",
	" ",
	"---",
	" ",
	" ",
	"Nos Familles",
	"Nos Amis",
	" ",
	" ",
	"---",
	" ",
	" ",
	"#demofr",
	"#codefr",
	"#traxfr",
	"#pixelfr",
	" ",
	" ",
	"---",
	" ",
	" ",
	"Orange Juice",
	"Gods",
	"Mandarine",
	"Orion",
	"Phelios",
	"Popsy Team",
	"Bomb",
	"Calodox",
	"Condense",
	"Eclipse",
	"Fool",
	"Revelation",
	"Glou",
	"Antishock",
	"Oror",
	"JFF",
	"Arf",
	"LucisMagia",
	"Wisdom",
	" ",
	" ",
	"---",
	" ",
	" ",
	"les zotres:",
	"Kaktux",
	"Tigrou",
	"Webep",
	"Eclipse",
	"Bul",
	" ",
	" ",
	"---",
	" ",
	" ",
	"All people we",
	"have forgot",
	" ",
	"We Love You",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	"(c) 2000 Knights",
	"Arf Studios",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
};

SDL_PixelFormat fmtARGB = {
	NULL,
	32,
	4,
	0, 0, 0, 0,
	16, 8, 0, 24,
	0x00ff0000, 0x0000ff00,
	0x000000ff, 0xff000000,
	0, 0
};

int MusicPosition()
{
	int pattern, row;
	pattern = module->sngpos;
	row = module->patpos;
	return pattern;
}

int MusicRow()
{
	int pattern, row;
	pattern = module->sngpos;
	row = module->patpos;
	return row;
}

inline void SetPixel(SDL_Surface * s, Uint32 x, Uint32 y, Uint32 color) 
{
	Uint32 *p;
	if (x >= s->w || y >= s->h)
		return;
	p = (Uint32 *) ( (Uint8*) (s->pixels) + y * s->pitch + x * s->format->BytesPerPixel );
	*p = color;
}


inline void SetPixel2x(SDL_Surface * s, Uint32 x, Uint32 y, Uint32 color) 
{
	Uint32 *p;
	if (x >= s->w-1 || y >= s->h-1)
		return;
	p = (Uint32 *) ( (Uint8*) (s->pixels) + y * s->pitch + x * 4 );
	*p = color;
	*(p+1) = color;
	*(p+s->pitch/4) = color;
	*(p+s->pitch/4+1) = color;
}


static Uint8 PCFONT[][8]=
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00,
  0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00,
  0x18, 0x7E, 0xC0, 0x7C, 0x06, 0xFC, 0x18, 0x00,
  0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00,
  0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00,
  0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00,
  0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00,
  0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00,
  0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30,
  0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,
  0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00,
  0x7C, 0xCE, 0xDE, 0xF6, 0xE6, 0xC6, 0x7C, 0x00,
  0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x00,
  0x78, 0xCC, 0x0C, 0x38, 0x60, 0xCC, 0xFC, 0x00,
  0x78, 0xCC, 0x0C, 0x38, 0x0C, 0xCC, 0x78, 0x00,
  0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x1E, 0x00,
  0xFC, 0xC0, 0xF8, 0x0C, 0x0C, 0xCC, 0x78, 0x00,
  0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x00,
  0xFC, 0xCC, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00,
  0x78, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0x78, 0x00,
  0x78, 0xCC, 0xCC, 0x7C, 0x0C, 0x18, 0x70, 0x00,
  0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30,
  0x18, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x18, 0x00,
  0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00,
  0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00,
  0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x00,
  0x7C, 0xC6, 0xDE, 0xDE, 0xDC, 0xC0, 0x7C, 0x00,
  0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00,
  0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00,
  0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00,
  0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00,
  0xFE, 0x62, 0x68, 0x78, 0x68, 0x62, 0xFE, 0x00,
  0xFE, 0x62, 0x68, 0x78, 0x68, 0x60, 0xF0, 0x00,
  0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3A, 0x00,
  0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x00,
  0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,
  0x1E, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00,
  0xE6, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0xE6, 0x00,
  0xF0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xFE, 0x00,
  0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00,
  0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00,
  0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00,
  0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00,
  0x7C, 0xC6, 0xC6, 0xC6, 0xD6, 0x7C, 0x0E, 0x00,
  0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x00,
  0x7C, 0xC6, 0xE0, 0x78, 0x0E, 0xC6, 0x7C, 0x00,
  0xFC, 0xB4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0x00,
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00,
  0xC6, 0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0x6C, 0x00,
  0xC6, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0xC6, 0x00,
  0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x78, 0x00,
  0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xFE, 0x00,
  0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00,
  0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00,
  0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00,
  0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
  0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76, 0x00,
  0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0xDC, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x00,
  0x1C, 0x0C, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00,
  0x38, 0x6C, 0x64, 0xF0, 0x60, 0x60, 0xF0, 0x00,
  0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8,
  0xE0, 0x60, 0x6C, 0x76, 0x66, 0x66, 0xE6, 0x00,
  0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,
  0x0C, 0x00, 0x1C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78,
  0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x00,
  0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,
  0x00, 0x00, 0xCC, 0xFE, 0xFE, 0xD6, 0xD6, 0x00,
  0x00, 0x00, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0x00,
  0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00,
  0x00, 0x00, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0,
  0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x1E,
  0x00, 0x00, 0xDC, 0x76, 0x62, 0x60, 0xF0, 0x00,
  0x00, 0x00, 0x7C, 0xC0, 0x70, 0x1C, 0xF8, 0x00,
  0x10, 0x30, 0xFC, 0x30, 0x30, 0x34, 0x18, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00,
  0x00, 0x00, 0xC6, 0xC6, 0xD6, 0xFE, 0x6C, 0x00,
  0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00,
  0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8,
  0x00, 0x00, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x00,
  0x1C, 0x30, 0x30, 0xE0, 0x30, 0x30, 0x1C, 0x00,
  0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,
  0xE0, 0x30, 0x30, 0x1C, 0x30, 0x30, 0xE0, 0x00,
  0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00
};

void SDL_DrawText(SDL_Surface * surface, int tx, int ty, char * text, Uint32 color)
{
	int x,y;
	char* p;

	p=text;
	while(*p)
	{
		int c = *p;
		if ((c<32)||(c>127))
		c = 127;
		c-=32;
		for(y=0;y<8;y++)
		{
			for(x=0;x<8;x++)
			{
				if (PCFONT[c][y]&(1<<x)) {
					SetPixel(surface, 8-x+tx, y+ty, color);
				}
			}
		}
		tx+=8;
		if (tx>surface->w)
		break;
		p++;
	}
}



inline Uint32 AlphaBlend(Uint32 d, Uint32 s, Uint8 a)
{
	Uint32 dalpha;
	Uint32 s1;
	Uint32 d1;

	dalpha = d & 0xff000000;
	s1 = s & 0xff00ff;
	d1 = d & 0xff00ff;
	d1 = (d1 + ((s1 - d1) * a >> 8)) & 0xff00ff;
	s &= 0xff00;
	d &= 0xff00;
	d = (d + ((s - d) * a >> 8)) & 0xff00;
	
	return (d1 | d | dalpha);

}

void DrawWipe(SDL_Surface * s1, SDL_Surface * s2, int pos, SDL_Surface * w, int smooth = 10)
{
	int x, y, i, n;
	Uint8 lookup[256];

	Uint32 *p1 = (Uint32 *) (s1->pixels);
	Uint32 *p2 = (Uint32 *) (s2->pixels);
	Uint8 *pw = (Uint8 *) (w->pixels);


	pos = 256 - ((pos * (256 + smooth)) / 256);

	for (i = 0; i < 256; i++) {
		n = 128 + (i - pos) * 128 / smooth;

		if (n < 0)
			n = 0;
		if (n > 255)
			n = 255;
		if (i < pos - smooth)
			n = 0;
		if (i > pos + smooth)
			n = 255;
		lookup[i] = n;
	}

	for (y = 0; y < 480; y++) {
		for (x = 0; x < 640; x++) {
			Uint8 c = lookup[*pw];

			if (c > 0) {
				if (c < 255) {
					*p1 = AlphaBlend(*p1, *p2, c);
				} else {
					*p1 = *p2;
				}
			}
			++pw;
			++p1;
			++p2;
		}
	}
}



void DrawPlasma(SDL_Surface * s)
{
	int x, y, i, n;
	Uint32 rx[1280];
	Uint32 gx[1280];
	Uint32 bx[1280];

	Uint32 ry[480];
	Uint32 gy[480];
	Uint32 by[480];

	Uint32 ry2[480];
	Uint32 gy2[480];
	Uint32 by2[480];

	Uint32 *p = (Uint32*) s->pixels;

	double f = frame;
	double zr = 0.5 + 0.2 * cos(f * 0.001);
	double zg = 0.5 + 0.4 * cos(f * 0.002);
	double zb = 0.5 + 0.5 * cos(f * 0.003);

	for (y = 0; y < 480; y++) {
		ry[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.01 + y * 0.08 * zr) * cos(f * 0.0003 + y * 0.001));
		gy[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.02 + y * 0.03 * zg) * cos(f * 0.0002 + y * 0.004));
		by[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.003 - y * 0.02 * zb) * cos(f * 0.0001 - y * 0.0002));

		ry2[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.02 + y * 0.02 * zr) * cos(f * 0.0001 + y * 0.006));
		gy2[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.004 + y * 0.01 * zg) * cos(f * 0.0002 + y * 0.0004));
		by2[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.01 - y * 0.03 * zb) * cos(f * 0.0003 - y * 0.003));
	}

	for (y = 0; y < 1280; y++) {
		rx[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.01 - y * 0.08 * zr) * cos(f * 0.0004 + y * 0.0008));
		gx[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.02 + y * 0.05 * zg) * cos(f * 0.0003 - y * 0.001));
		bx[y] = (Uint32) (64.0 + 64.0 * sin(f * 0.003 + y * 0.02 * zb) * cos(f * 0.0002 + y * 0.003));
	}

	for (y = 0; y < 480; y++) {
		p = (Uint32*) (s->pixels) + y * s->pitch / 4;
		Uint32 lr = (ry[y] + ry2[y]) / 2;
		Uint32 lg = (gy[y] + gy2[y]) / 2;
		Uint32 lb = (by[y] + by2[y]) / 2;
		Uint32 pr, pb, pg;;
		pr = (Uint32) (32.0 + 32.0 * cos(f * 0.001 + y * 0.004));
		pg = (Uint32) (32.0 + 32.0 * cos(f * 0.0002 + y * 0.002));
		pb = (Uint32) (32.0 + 32.0 * cos(f * 0.0003 + y * 0.01));

		for (x = 640; x > 0; x--) {
			*p = ((lr + rx[x + pr]) << 16) |
			 	((lg + gx[x + pg])  << 8)  |
			 	(lb + bx[x + pb]);
			++p;
		}
	}
}



int random_table[32][1024];

void DrawRotoZoom(SDL_Surface * dest, double zoom, float angle,
					SDL_Surface * map, float ox = 0, float oy = 0) {
	const float F16 = 65536.0 /*/256 */ ;


	int px, py;
	int dx, dy;
	float gpx, gpy;
	float gdx, gdy;

	int x, y;

	float f = angle * 0.02454369261f;

	gpx = 256.0 + zoom * 360.0 * cos(-f + 3.0 * 3.14 / 4.0) + ox;
	gpy = 256.0 - zoom * 360.0 * sin(-f + 3.0 * 3.14 / 4.0) + oy;

	gdx = zoom * cos(f + 3.14 / 2.0);
	gdy = zoom * sin(f + 3.14 / 2.0);

	for (y = 0; y < 480; y++) {
		px = (int) (gpx * F16);
		py = (int) (gpy * F16);

		gpx += gdx;
		gpy += gdy;

		dx = (int) (zoom * cos(f) * F16);
		dy = (int) (zoom * sin(f) * F16);

		//px += F16 * 8.0 * cos( (y+frame) *0.0001)* cos( (y+frame) *0.003);
		//py += F16 * 8.0 * sin( (y+frame) *0.0002)* cos( (y+frame) *0.005);

		Uint32 *p = &((Uint32 *) dest->pixels)[512 * y];

		int *random = random_table[rand() & 31];

		for (x = 0; x < 512; x++) {
			int rx, ry;

			rx = px >> 16;
			ry = py >> 16;
			px += dx;
			py += dy;

			rx += *random;
			random++;
			ry += *random;
			random++;

			*p = ((Uint32 *) map->pixels)[(512 * ry + rx) & 0x3ffff];
			++p;
		}
	}
}

const int NUMSNOW = 1000;

struct Snow {
	double x, y, z;
};

Snow snow[NUMSNOW];

gzFile zDump;

Uint32 dump=0;
int dump_frame=0;

int already_dumping = 0;
SDL_Surface *save;
	
int DumpFrame_(Uint32 diff)
{
	if (already_dumping)
		return 0;
	already_dumping = 1;
	
	dump = SDL_GetTicks();
	
	/*if (diff > 30)
		printf("drop: #%d -> %3d ms\n", frame_count, diff);*/
	
	/*if (dump>=30)*/ {
		//char s[256];
		//sprintf(s, "dump/%08d-%d.bmp",dump_frame++, int(dump));
		//SDL_BlitSurface(screen,0,zoomed,0);
		//SDL_SaveBMP(zoomed, s);
		
		int x,y;
		Uint32 dump_buffer[320*240];
		Uint32 *dest=dump_buffer;
		
		for(y=0;y<240;y++) {
			Uint32 *p = (Uint32*) ((Uint8*) save->pixels + 2*y * save->pitch);
			
			for(x=0;x<320;x++) {
				// fast but ugly and unprecise bilinear downsampling
				Uint32 a,b,c,d;
				a = *p;
				b = *(p+1);
				c = *(p+640);
				d = *(p+640+1);
				
				a >>= 1;
				b >>= 1;
				c >>= 1;
				d >>= 1;
				
				a &= 0x007f7f7f;
				b &= 0x007f7f7f;
				c &= 0x007f7f7f;
				d &= 0x007f7f7f;
				
				a += c;
				b += d;
				
				a >>= 1;
				b >>= 1;
				a &= 0x007f7f7f;
				b &= 0x007f7f7f;

				*dest = a+b + 0xff000000;
				dest++;
				p += 2;
			}
		}
		if (zDump) {
			gzwrite(zDump, &dump, sizeof(dump));
			gzwrite(zDump, dump_buffer, sizeof(dump_buffer));
		}

		//gzwrite(zDump, (Uint8*)screen->pixels+0*640*480, 640*480*1);
		dump=0;
	}
	already_dumping = 0;
	return 0;
}

int DumpFrame(void*unused)
{
	Uint32 c;
	Uint32 old=0;
	Uint32 start, end;
	Uint32 next;
	Uint32 prev;
	
	while (1) {
		if (c >= 0) {
			start = SDL_GetTicks();
			next = start + 40-4;
			if (start - prev > 40) {
				printf("dropped: %d ms\n",start-prev);	
			}
			prev = start;
			DumpFrame_(c);
			end = SDL_GetTicks();
			
			/*while (SDL_GetTicks() <= next) {
				SDL_Delay(0);
			}*/
			
			//printf("drop: %d ms \n", end-start);
			
			
			//if (c > 40)
				//printf("drop: #%d -> %3d ms\n", frame_count, c);
			c = 0;
		}
		//SDL_Delay(0);
		Uint32 t = SDL_GetTicks();
		c += t - old;
		old = t;
	}
}

class Demo {
  public:

	SDL_Surface *bmpLayer1;
	SDL_Surface *bmpLayer2;
	SDL_Surface *bmpLayer3;
	SDL_Surface *bmpLayer4;
	SDL_Surface *bmpBonne[BLUR_STEPS];
	SDL_Surface *bmpKnights[BLUR_STEPS];
	SDL_Surface *bmpTitle[BLUR_STEPS];
	SDL_Surface *bmpColor;
	SDL_Surface *bmpLogo;
	SDL_Surface *bmpBack;
	SDL_Surface *bmpBack2;
	SDL_Surface *bmpLittle;
	SDL_Surface *bmpPapa;

	SDL_Surface *bmpFont[128 - 32];

	SDL_Surface *bmpWipe1;
		
	SDL_Surface *_screen;


	
	Demo() 
	{

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0) {
			fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
			exit(1);
		}
		putenv("SDL_VIDEO_CENTERED=1");
		screen = SDL_SetVideoMode(ScreenWidth, ScreenHeight, 32, SDL_SWSURFACE);
		if ( screen == NULL) {
			fputs("Couldn't set video mode:\n", stderr);
			fputs(SDL_GetError(), stderr);
			fputs("\n", stderr);
			SDL_Quit();
			exit(1);
		}

		/*if (fullscreen) {
			SDL_ShowCursor(SDL_FALSE);
		}*/

		//SDL_DrawText(screen, 10, 10, "Knights does it better", 0xffffffff);
		//SDL_DrawText(screen, 10, 470, "Yes! Knights Re-United !!", 0xffffffff);
		SDL_DrawText(screen, 180, 230, "   calculating l'age de ta mere", 0xffffffff);
		SDL_DrawText(screen, 180, 240, "ca risque de prendre un moment... ", 0xffffffff);
		SDL_Flip(screen);

		SDL_Surface *temp;

		SDL_EnableUNICODE(SDL_TRUE);
		SDL_WM_SetCaption("Knights does it better", NULL);
		SDL_Flip(screen);

		bmpWipe1 = IMG_Load("data/wipe1.png");

		temp = IMG_Load("data/color.png");
		bmpColor = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/back.png");
		bmpBack = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/back2.png");
		bmpBack2 = SDL_DisplayFormatAlpha(temp);
		SDL_SetAlpha(bmpBack2, SDL_SRCALPHA | SDL_RLEACCEL, 255);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/xmas2.png");
		bmpPapa = SDL_DisplayFormat(temp);
		SDL_SetColorKey(bmpPapa, SDL_SRCCOLORKEY, 0xff00ff);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/logo.png");
		bmpLogo = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/fond01.png");
		bmpLayer1 = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/fond02.png");
		bmpLayer2 = SDL_DisplayFormatAlpha(temp);
		SDL_SetAlpha(bmpLayer2, SDL_SRCALPHA | SDL_RLEACCEL, 255);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/fond03.png");
		bmpLayer3 = SDL_DisplayFormatAlpha(temp);
		SDL_SetAlpha(bmpLayer3, SDL_SRCALPHA | SDL_RLEACCEL, 255);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/fond04.png");
		bmpLayer4 = SDL_DisplayFormatAlpha(temp);
		SDL_SetAlpha(bmpLayer4, SDL_SRCALPHA | SDL_RLEACCEL, 255);
		SDL_FreeSurface(temp);

		temp = IMG_Load("data/littlekhristmas.png");
		bmpLittle = SDL_DisplayFormatAlpha(temp);
		SDL_SetAlpha(bmpLittle, SDL_SRCALPHA | SDL_RLEACCEL, 255);
		SDL_FreeSurface(temp);

		int i;

		for (i = 0; i < NUMSNOW; i++) {
			snow[i].x = rand() % 640;
			snow[i].y = 480 + i;
			snow[i].z = 30 + rand() % 40;
		}

		SDL_Flip(screen);

		SDL_Surface *bmpTemp;

		bmpTemp = IMG_Load("data/font.png");
		for (i = 0; i < 128 - 32; i++) {
			SDL_Rect sr = {32*i, 0, 32, 34};
			SDL_Rect dr = {0, 0, 32, 34};
			temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 32, 34, 32
				, 0xff0000, 0xff00, 0xff, 0xff000000);
			SDL_BlitSurface(bmpTemp, &sr, temp, &dr);
			bmpFont[i] = SDL_DisplayFormat(temp);
			SDL_FreeSurface(temp);
			SDL_SetColorKey(bmpFont[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0x00ff00ff);
		}

		temp = IMG_Load("data/khristmas.png");
		//SDL_DisplayFormat(temp);

		//bmpTitle[0] = IMG_Load("data/khristmas.png");
		bmpTitle[0] = SDL_DisplayFormatAlpha(temp);
		SDL_FreeSurface(temp);
		
		for (i = 1; i < BLUR_STEPS; i++) {
			temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32
				, 0xff, 0xff00, 0xff0000, 0xff000000);
			bmpTitle[i] = SDL_DisplayFormatAlpha(temp);
			SDL_FreeSurface(temp);
			SDL_FillRect(bmpTitle[i], 0, 0);
			GaussianBlur(bmpTitle[i], bmpTitle[i - 1], 1 + i * 8);
			SDL_SetAlpha(bmpTitle[i], SDL_SRCALPHA | SDL_RLEACCEL, 255);
		}
		SDL_SetAlpha(bmpTitle[0], SDL_SRCALPHA | SDL_RLEACCEL, 255);

		temp = IMG_Load("data/knights.png");
		bmpKnights[0] = SDL_DisplayFormatAlpha(temp);
		SDL_FreeSurface(temp);
		for (i = 1; i < BLUR_STEPS; i++) {
			temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32
				, 0xff, 0xff00, 0xff0000, 0xff000000);
			bmpKnights[i] = SDL_DisplayFormatAlpha(temp);
			SDL_FreeSurface(temp);
			SDL_FillRect(bmpKnights[i], 0, 0);
			GaussianBlur(bmpKnights[i], bmpKnights[i - 1], 1 + i * 8);
			SDL_SetAlpha(bmpKnights[i], SDL_SRCALPHA | SDL_RLEACCEL, 255);
		}
		SDL_SetAlpha(bmpKnights[0], SDL_SRCALPHA | SDL_RLEACCEL, 255);

		temp = IMG_Load("data/xmasmum.png");
		bmpBonne[0] = SDL_DisplayFormatAlpha(temp);
		SDL_FreeSurface(temp);
		for (i = 1; i < BLUR_STEPS; i++) {
			temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32
				, 0xff, 0xff00, 0xff0000, 0xff000000);
			bmpBonne[i] = SDL_DisplayFormatAlpha(temp);
			SDL_FreeSurface(temp);
			SDL_FillRect(bmpBonne[i], 0, 0);
			GaussianBlur(bmpBonne[i], bmpBonne[i - 1], 1 + i * 2);
			SDL_SetAlpha(bmpBonne[i], SDL_SRCALPHA | SDL_RLEACCEL, 255);
		}
		SDL_SetAlpha(bmpBonne[0], SDL_SRCALPHA | SDL_RLEACCEL, 255);

		temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 512, 32
				, 0xff0000, 0xff00, 0xff, 0xff000000);
		buffer[0] = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);
		
		temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 512, 32
				, 0xff0000, 0xff00, 0xff, 0xff000000);
		buffer[1] = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);

		temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32
				, 0xff0000, 0xff00, 0xff, 0xff000000);
		_screen = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);
		
		temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32
				, 0xff0000, 0xff00, 0xff, 0x000000);
		save = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);
		SDL_SetAlpha(screen, 0, 255);
	}

	void DrawText(SDL_Surface * su, int x, int y, char *s) {
		int tx = x - (strlen(s) * 26 / 2);
		char *p;

		for (p = s; *p; p++) {
			int c = (*p) - ' ';
			SDL_Rect dr = {tx, y, 32, 34};
			SDL_BlitSurface(bmpFont[c], 0, su, &dr);
			tx += 26;
		}
	}

	void CalcSnow(float advance = 1.0f) {
		int i;

		for (i = 0; i < NUMSNOW; i++) {
			snow[i].y += advance * delta * 0.001 * snow[i].z;
			if (snow[i].y >= 480)
				snow[i].y = -(rand() % NUMSNOW);
		}
	}

	void DrawSnow() {
		int i;

		for (i = 0; i < NUMSNOW; i++) {
			SetPixel(screen,
					 Uint32(snow[i].x +
					 10.0 * sin(snow[i].z * frame * 0.00004)),
					 Uint32(snow[i].y), 0xffffffff);
		}
	}

	void DrawSnow2x() {
		int i;

		for (i = 0; i < NUMSNOW; i++) {
			SetPixel2x(screen,
					   Uint32(640 - snow[i].x +
					   10.0 * sin(snow[i].z * frame * 0.00004)),
					   Uint32(snow[i].y), 0xffffffff);
		}
	}

	void GaussianBlur(SDL_Surface * dest, SDL_Surface * bmp, int depth) {
		int x, y;

		for (y = 0+4; y < bmp->h-4; y++) {
			Uint32 *p = (Uint32 *) ((Uint8 *)bmp->pixels + y * bmp->pitch);
			Uint32 *d = (Uint32 *) ((Uint8 *)dest->pixels + y * bmp->pitch);

			for (x = 8; x < bmp->w - 8; x++) {
				Uint32 a, b, c;

				if (*p) {
					a = *(p - 4);
					b = *(p + 4);
					a >>= 1;
					a &= 0x7f7f7f7f;
					b >>= 1;
					b &= 0x7f7f7f7f;
					a += b;

					b = *(p - 2);
					c = *(p + 2);
					b >>= 1;
					b &= 0x7f7f7f7f;
					c >>= 1;
					c &= 0x7f7f7f7f;
					b += c;

					a >>= 1;
					a &= 0x7f7f7f7f;
					b >>= 1;
					b &= 0x7f7f7f7f;
					a += b;

					b = *(p - bmp->w);
					c = *(p + bmp->w);
					b >>= 1;
					b &= 0x7f7f7f7f;
					c >>= 1;
					c &= 0x7f7f7f7f;
					b += c;

					a >>= 1;
					a &= 0x7f7f7f7f;
					b >>= 1;
					b &= 0x7f7f7f7f;
					a += b;

					*d = a;
				} else
					*d = 0;
				d++;
				p++;
			}
		}
	}

	void GaussianBlur_(SDL_Surface * dest, SDL_Surface * bmp, int depth) {
		int x, y;

		for (y = 0; y < bmp->h; y++) {
			Uint32 *p = (Uint32 *) ((Uint8 *)bmp->pixels + y * bmp->pitch);
			Uint32 *d = (Uint32 *) ((Uint8 *)dest->pixels + y * bmp->pitch);


			for (x = 0; x < bmp->w; x++) {
				int i = 0;

				int r = 0, g = 0, b = 0, a = 0;


//        if (*(p+x)>0)
				{
					for (i = -depth; i < depth; i++) {
						int bx;

						bx = x + i;

						if ((bx < bmp->w) && (bx >= 0)) {
							Uint32 d_ = *(p + bx);

							b += d_ & 255;
							d_ >>= 8;
							g += d_ & 255;
							d_ >>= 8;
							r += d_ & 255;
							d_ >>= 8;
							a += d_ & 255;
						}
					}
					r /= depth * 2;
					g /= depth * 2;
					b /= depth * 2;
					a /= depth * 2;
				}
				*d = (a << 24) | (r << 16) | (g << 8) | b;
				d++;
			}
		}
	}


	void PartIntro() {

		int p;

		p = int( BLUR_STEPS/2.0 + BLUR_STEPS/2.0 * sin(frame * 0.005));
		if (p < 0)
			p = 0;

		if (MusicPosition() >= 2) {
			SDL_BlitSurface(bmpLayer1,0,screen,0);
			SDL_BlitSurface(bmpLayer2,0,screen,0);
			DrawSnow();
			SDL_BlitSurface(bmpLayer3,0,screen,0);
			DrawSnow2x();

			static double x = 0;
			static int pp = 0;
			
			SDL_Rect r = {int(640-x),0,640,480};
			SDL_BlitSurface(bmpLayer4,0,screen, &r);

			if (x < 640) {
				x += delta;
				pp++;
				pp += p;
				if (pp >= BLUR_STEPS)
					pp = BLUR_STEPS-1;
				SDL_Rect r = {int(x),0,640,480};
				SDL_BlitSurface(bmpKnights[pp],0,screen, &r);
			}
			
			if ((MusicPosition() >= 5) && (MusicPosition() < 7)) {
				if (((MusicRow() / 4) % 4) == 0)
					DrawText(screen, 320, 230, "Kassoulet");
				if (((MusicRow() / 4) % 4) == 1)
					DrawText(screen, 320, 230, "Marblemad");
				if (((MusicRow() / 4) % 4) == 2)
					DrawText(screen, 320, 230, "Stuff");
				if (((MusicRow() / 4) % 4) == 3)
					DrawText(screen, 320, 230, "LogicDream");
			}
			if (MusicPosition() >= 7) {
				static float x = -640;

				x += delta;
				if (x > 0)
					x = 0;
				SDL_Rect r = {int(x),0,640,480};
				SDL_BlitSurface(bmpTitle[p],0,screen, &r);
			}
		} else {
			SDL_BlitSurface(bmpLayer1,0,screen,0);
			SDL_BlitSurface(bmpLayer2,0,screen,0);
			SDL_BlitSurface(bmpLayer3,0,screen,0);
			SDL_BlitSurface(bmpKnights[p],0,screen, 0);
		}

	}

	void PartRoto() {
		static double d = 0;
		static int f = 0;
		int i;

		d += delta;
		if (d >= 30) {
			f++;
			DrawRotoZoom(buffer[f & 1], 0.98,
						   2.0 * sin(frame * 0.0002),
						   buffer[(f + 1) & 1], -1, -1);
			d = 0;
		}

		static int g = 0;
		static int o = 0;

		if (MusicRow() / 4 != o) {
			g++;
			o = MusicRow() / 4;
		}


		SDL_BlitSurface(buffer[f & 1],0,screen,0);
		if (MusicPosition() < 18)
			for (i = 0; i < 50; i++) {
				SDL_Rect sr = {rand() & 511, 0, 1, 1};
				SDL_Rect dr = {rand() & 511, rand() & 511, 1, 1};
				SDL_BlitSurface(bmpColor,&sr,buffer[f & 1], &dr);
			}
		for (i = 0; i < 100; i++) {
			SetPixel(buffer[f & 1], rand() % 640, rand() % 480, 0);
		}
		if (MusicPosition() > 17) {
			DrawText(screen, 256, 230, greets[g % 19]);
		}
		
		SDL_Rect r = {512,0,128,480};
		SDL_BlitSurface(bmpLogo,0,screen, &r);

		DrawSnow();
		SDL_BlitSurface(bmpLittle,0,screen,0);
		DrawSnow2x();
	}

	void PartPlasma(SDL_Surface * screen) {
		DrawPlasma(screen);

		static double t = 0;

		if (MusicPosition() > 20)
			t +=::delta * 0.3;
		if (t > 1000)
			t = 1000;

		int y;

		for (y = 0; y < 400; y++) {
			if ((rand() % 1000) < t) {
				SDL_Rect sr = {0, y, 280, 1};
				SDL_Rect dr = {int(320 + 16.0 * sin(frame * 0.01 + y * 0.01)), 40 + y, 280, 1};
				SDL_BlitSurface(bmpPapa,&sr,screen,&dr);
			}
		}
		DrawSnow();
		SDL_BlitSurface(bmpLittle,0,screen,0);
		DrawSnow2x();
	}

	void PartScroll() {
		static double y = 480;
		static double x = 0;
		static double f = 0;
		static int d = 1;

		f += delta;

		if (f > 6000.0) {
			x += d * delta;
			if (x > 320) {
				x = 320;
				d = -1;
				f = 0.0;
			}
			if (x < 0) {
				x = 0;
				d = 1;
				f = 0.0;
			}
		}


		if (y == 480) {
			int i;

			for (i = 0; i < 128 - 32; i++)
				SDL_SetAlpha(bmpFont[i],SDL_RLEACCEL,128);
		}

		y -= delta * 0.04;

		SDL_BlitSurface(bmpBack,0,screen,0);

		int i;
		char *p;

		for (i = 0; i < 5000; i++) {
			p = scroll[i];
			if (!(*p))
				break;
			int py;

			py = int(y) + i * 34;
			if ((py > -35) && (py < 480)) {
				DrawText(screen, int(320 - 80 + x / 2), py, p);
			}
		}

		SDL_BlitSurface(bmpBack2,0,screen,0);
		
		DrawSnow();
		DrawSnow2x();

		int b;

		b = int(BLUR_STEPS/2.0 * sin(frame * 0.002));
		if (b < 0)
			b = 0;
		SDL_Rect r = {int(160 - x), 0,640,480};
		SDL_BlitSurface(bmpBonne[b],0,screen, &r);
		
		SDL_Rect dr = {int(160 - x), 0, 640, 480};
		SDL_BlitSurface(bmpBonne[b],0,screen,&dr);

	}



	void Run() {
		bool end = false;
		int f;
		int i;
		int d = 0;

		/*Player_SetPosition(24);
		Player_SetSpeed(6);		
		Player_SetTempo(145);*/
		//Player_SetVolume(0);
		
		while (!end) {
			Uint8 *keys;
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					end = true;
				}
				if (event.type == SDL_KEYDOWN) {
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						end = true;
					}
				}
			}
			keys = SDL_GetKeyState(NULL);

			if (MusicPosition() >= 2)
				CalcSnow();

			if (MusicPosition() < 11) {
				PartIntro();
			}

			if ((MusicPosition() >= 11) && (MusicPosition() < 19)) {
				static float w = 0;

				w += delta * 0.1;

				if (w < 255) {
					PartIntro();
					DrawWipe(screen, _screen, int(w), bmpWipe1, 20);
					DrawSnow();
					DrawSnow2x();
				} else {
					PartRoto();
				}
			}

			if ((MusicPosition() >= 19) && (MusicPosition() < 24)) {
				static float w = 0;

				if (w < 255) {
					w += delta * 0.1;
					PartRoto();
					PartPlasma(_screen);
					DrawWipe(screen, _screen, int(w), bmpWipe1, 20);
					DrawSnow();
					DrawSnow2x();
				} else
					PartPlasma(screen);
			}

			if (MusicPosition() >= 24) {
				static float w = 0;

				if (w < 255) {
					w += delta * 0.1;
					PartPlasma(screen);
					DrawWipe(screen, bmpBack, int(w), bmpWipe1, 20);
					DrawSnow();
					DrawSnow2x();
				} else
					PartScroll();
			}

			frame_count++;
			
			static Uint32 old=0;
			Uint32 ticks;
			static Uint32 tt=0;

			
			ticks = SDL_GetTicks();
			
			diff = ticks - old;
			
			delta = delta*0.8 + diff*0.2;
			frame += delta;
			tt+=diff;

			fps = 1000.0 / delta;
			old = ticks;
			
			if (keys[SDLK_SPACE] == SDL_PRESSED) {
				char str[160];

				sprintf(str, "%02x:%02x - %3d fps - %.2f ms",MusicPosition(), MusicRow(),
					int(fps), delta);
				SDL_DrawText(screen, 0, 0, str, 0);
				SDL_DrawText(screen, 0, 2, str, 0);
				SDL_DrawText(screen, 2, 0, str, 0);
				SDL_DrawText(screen, 2, 2, str, 0);
				SDL_DrawText(screen, 1, 1, str, 0xffffffff);
			}
		
			//DumpFrame(0);
			//SDL_BlitSurface(screen,0,save,0);

			SDL_Flip(screen);	
			
			
			if (keys[SDLK_TAB] == SDL_PRESSED) {
				SDL_Delay(100);
			}
		}
	}

};


int main(int argc, char **argv) 
{
	//zDump = gzopen("dump/dump.gz", "wb3");
	Demo demo;

	OpenMusic("data/noel2.xm");

	int i, j;

	for (j = 0; j < 32; j++)
		for (i = 0; i < 1024; i++)
			random_table[j][i] = (2 - (rand() % 4)) / 1;

	//SDL_CreateThread(DumpFrame, 0);

	//SDL_ShowCursor(0);
	demo.Run();
	
	/*SDL_Delay(100);
	gzFile z = zDump;
	zDump = 0;
	SDL_Delay(100);
	gzclose(z);
	SDL_Delay(100);*/
	
	CloseMusic();
	SDL_Quit();
	

	return 0;
}
