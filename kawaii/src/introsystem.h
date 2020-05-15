/* Schtroumpf - the intro - (c) 2004 Gautier Portet < kassoulet @ no-log . org >
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
 * IntroSystem main header
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SDL.h"
#include "SDL_opengl.h"

typedef GLuint Texture;

#include "bitmap.h"
#include "config.h"

#ifdef USE_MIKMOD
#else
#define __COMPILE_MINIFMOD__
#include "minifmod/minifmod.h"
#endif

#define PI   	3.141592653589793
#define TWOPI	(2.0*PI)

/*typedef enum bool {
	false,
	true
} bool;*/

typedef enum {
	false,
	true
} bool;


typedef enum KEY {
	KEY_NONE,
	KEY_ESCAPE,
	KEY_RETURN,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
} KEY;

extern int ScreenWidth;
extern int ScreenHeight;
//extern int BufferWidth;
//extern int BufferHeight;

//extern int LowDetails;
extern int Fullscreen;

/* milliseconds since program start */
extern Uint32 ticks;

/* milliseconds since previous frame */
extern double ms;

#define MS_SIZE 128
extern Uint32 ms_array[MS_SIZE];

/* number of frames since program start */
extern Uint32 frames;

/*
  delta=1 when running at 50fps, 0.5 at 100fps and 2 at 25fps
  so we can have a speed independant increment
*/
extern double delta;

/* timer is a counter increased with delta */
extern double timer;

typedef struct {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} Color_;

typedef struct {
	float x;
	float y;
} Point;

/* General setup */
void OpenSystem();
void CloseSystem();

/* Is the user trying to Escape ? fucker :) */
int CheckUserBreak();
KEY GetKeyPressed();

/* Videomode */
void OpenVideo(int width, int height, int fullscreen, int multisample);
void CloseVideo();
void Flip();

/* Software Surfaces, stored in main memory */

typedef struct Surface {
	int width;
	int height;
	int bpp;  /* bytes */
	Uint8 *pixels;
} Surface;

Surface * CreateSurface(int width, int height, int bpp);
void FreeSurface(Surface *surface);
void FillSurface(Surface *surface, Uint32 color);
void BlitSurface(Surface *dest, Surface *source, int x, int y); /*TODO*/

/* Load a 8bpp RLE packed bitmap */
Surface* LoadRLEBitmap(RLEBitmap *rle);

/* Hardware Textures */

GLuint CreateTexture(int width, int height);
void FreeTexture(GLuint texture);
void UploadTexture(GLuint texture, Surface *surface);
void UploadTextureMipmap(GLuint texture, Surface *surface);
void UploadTextureLuminance(GLuint texture, Surface *surface);
void UploadTextureAlpha(GLuint texture, Surface *surface);
void UploadTextureSub(GLuint texture, Surface *surface, int x, int y);

/* BMP support, only when linked with SDL */
Surface * LoadBMP(char * filename);
void SaveBMP(Surface * bmp, char * filename);

/* Misc functions */

int NextPowerOfTwo(int number);
/* Set an OpenGL 2D mode */
void Mode2D();
/* Sinewave: return a sinus between min and max, with period and offset */
float Sine(float min, float max, float period, float offset);
/* Sinewave, with a custom time reference */
float SineEx(double timer, float min, float max, float period, float offset);
/* Perlin noise in a surface */
void FillNoise(Surface * s, int octaves, float persistence, float period, float nx, float ny, float nz);
/* Threshold - smoothing: size of the smoothed transition, pixel will be in range min->max  */
void Threshold(Surface * s, int threshold, int smoothing, int min, int max);
/* Add a transparent shadow, placed at sx, sy */
void AddShadow(Surface * surface, int sx, int sy);
/* OpenGL cube */
void GenCube();

void ColorHSV( float h, float s, float v, float *r, float *g, float *b);

/* AdvanceMame ScaleX */
Surface * Scale2x(Surface * src);
Surface * Scale4x(Surface * src);
Surface * Scale8x(Surface * src);

/* Scale a surface with bilinear interpolation */
Surface * Scale(Surface * src, float scale);
/* Simple blur */
void Blur(Surface * src, int repeat);

void Noisify(Surface * s, int noise);
void AddBorder(Surface * surface, Uint32 color);


/* Floating point random */
float frand();

/* Perlin ease curve (range 0.0->1.0)*/
static inline float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

/* linear Iterpolation */
static inline float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

/* 3D Perlin noise */
float noise(float x, float y, float z);

/* useful macros */
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define CLAMP(x,a,b) (MIN(MAX(a,x),b))

/* Text Functions */

extern GLuint texFont;

typedef enum TextPlacement {
	TEXT_LEFT,
	TEXT_CENTER,
	TEXT_RIGHT
} TextPlacement;

void DrawString(float x, float y, char* str, TextPlacement place, GLint texFont, float size);
int GetStringWidth(char* str);
void InitFont(Surface * bmpFont);

/* Intro Framework */

typedef void (*VOIDFUNC)();
typedef void (*DRAWFUNC)(double time, double delta, int position);

typedef struct DemoPart {
	int start;
	int end;

	DRAWFUNC Draw;
	VOIDFUNC Open;
	VOIDFUNC Close;
} DemoPart;

/* XM Player */

void OpenMusic(void * pModule, int size);
void CloseMusic();
int GetMusicPosition();
void SetMusicPosition(int pattern);
//int GetSynchro();

bool GetSynchro(int* list);



