/* Kawaii (c) 2004 Gautier Portet < kassoulet @ no-log . org >
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "introsystem.h"

#define RAINBOW_SIZE 1024
#define RAINBOW_RSIZE 768
Uint32 bmpRainbow[RAINBOW_SIZE];
Texture texRainbow;

#define BLUR_STEPS 8

Surface* bmpBunny;
Texture texBunny[BLUR_STEPS];



static float doomwipe[1024];
#define DOOMWIPE_SIZE 2
static bool bDoomwipe = true;

static bool  showLogo = false;
static float fallingPos = -200;
static float fallingSpeed = 20;

static float ending = 0;

#define STARS_MAX 256
typedef struct Star {
	float x, y;
	float speed;
	float size;
	float rotation;
	float rotationSpeed;
} Star;
static Star stars[STARS_MAX];

GLuint listStar;

void partStart_Draw(double time, double delta, int position, bool wipe);

void StarsInit(Star *stars)
{
	float arc = TWOPI / 10.0f;
	int i, j;
	float pike = 0.3;
	float h = 1.0f + pike;
	float l = 1.0f - pike;
	float angle;

	for (i = 0; i < STARS_MAX; i++) {
		stars->x = rand() % 1024;
		stars->y = 64 + 768 + (rand() % 768);
		stars->speed = 0.5f + frand();
		stars->size  = 24 + (rand() % 24);
		stars->rotation = rand() % 360;
		stars->rotationSpeed = 0.5f - frand();
		if (fabs(stars->rotationSpeed) < 0.1) {
			stars->rotationSpeed *= 5.0f;
		}
		stars++;
	}

	listStar = glGenLists(1);
	glNewList(listStar, GL_COMPILE);


	glBegin(GL_TRIANGLE_FAN);

	glColor4f(1, 1, 0, 0.5);

	glVertex2f(0, 0);
	for (j = 0; j < 5; j++) {
		angle = arc * j * 2;
		glVertex2f(h*cos(angle), h*sin(angle));
		glVertex2f(l*cos(angle + arc), l*sin(angle + arc));
	}
	glVertex2f(h*cos(angle + arc*2), 	h*sin(angle + arc*2));

	glEnd();

	//glScalef(1.1,1.1,1);

	glBegin(GL_LINE_STRIP);
	glColor4f(1, 1, 1, 0.5);

	for (j = 0; j < 5; j++) {
		angle = arc * j * 2;
		glVertex2f(h*cos(angle), h*sin(angle));
		glVertex2f(l*cos(angle + arc), l*sin(angle + arc));
	}
	glVertex2f(h*cos(angle + arc*2), 	h*sin(angle + arc*2));

	glEnd();

	glEndList();
}

void StarsDraw(Star *stars)
{
	float arc = TWOPI / 10.0f;
	int i, j;
	float x, y;
	float radius, angle;


	glLineWidth(2);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	for (i = 0; i < STARS_MAX; i++) {

		radius = stars->size;
		angle  = stars->rotation;
		x = stars->x;
		y = stars->y;


		glLoadIdentity();
		glTranslatef(x, y, 0);
		glRotatef(angle, 0, 0, 1);
		glScalef(radius, radius, 1);

		glCallList(listStar);

		stars->y -= stars->speed * delta * 4.0f;
		stars->rotation += stars->rotationSpeed * delta * 5.0f;

		if (stars->y < -radius ) {
			stars->y += 768 + radius * 2;
		}

		stars++;
	}
}


static char subtitle[][16] = {
	{"an"},
	{"an in"},
	{"an intr"},
	{"an intro"},
	{"an intro i"},
	{"an intro in"},
	{"an intro in 6"},
	{"an intro in 64"},
	{"an intro in 64K"},
};
static char displayed[32];
static int _index = 0;
static float alphakitty = 0;

void DrawTitle(double time, double delta, int position)
{
	int i;
	int synchro[] = {0x063c,	0x0714,	0x0718,	0x071c,	0x0730,	0x0738,	0x0800,	0x0806,	0x080c, 0};



	if ( GetSynchro(synchro) ) {
		strcpy(displayed, subtitle[_index]);
		_index++;
	}

	for (i = 0; i < RAINBOW_RSIZE; i++) {
		float r, g, b, v, f;
		int ir, ig, ib;

		r = g = b = (float)i / RAINBOW_RSIZE;

		/* luminosity */
		v = 0.8 * fabs(sin((TWOPI * i + ticks * -0.3) / 16));

		/* fade */
		f = CLAMP(i / 32.0f, 0, 1);
		f *= CLAMP((768 - i) / 32.0f, 0, 1);
		v *= f;

		/* hue+luminosity */
		ColorHSV((i + ticks*0.08 + fallingPos + 200)*360.0f / RAINBOW_RSIZE, 1, v, &r, &g, &b);
		ir = r * 255.0f;
		ig = g * 255.0f;
		ib = b * 255.0f;

		bmpRainbow[i] =
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		    (255 << 24) + (ir << 16) + (ig << 8) + (ib << 0);
#else
		    (255 << 0) + (ir << 8) + (ig << 16) + (ib << 24);
#endif
	}

	glBindTexture(GL_TEXTURE_1D, texRainbow);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, RAINBOW_SIZE,	0, GL_RGBA, GL_UNSIGNED_BYTE, bmpRainbow);
	Mode2D();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, texRainbow);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBegin(GL_TRIANGLE_STRIP);

	glColor4f(1, 1, 1, 1);

	glTexCoord1f(0);
	glVertex2i(0,   0);
	glVertex2i(1024, 0  );

	glTexCoord1f((float)RAINBOW_RSIZE / RAINBOW_SIZE);
	glVertex2i(0,   768);
	glVertex2i(1024, 768);
	glEnd();

	glDisable(GL_TEXTURE_1D);



	if ( !showLogo && (position > 0x0430) ) {
		showLogo = true;
	}

	if ( position > 0x0c00 ) {
		ending += delta * 0.9;
	}

	if (showLogo) {
		int i, x, y;
		StarsDraw(stars);

		glEnable(GL_TEXTURE_2D);

		Mode2D();
		DrawString(512, 384 + 128, displayed, TEXT_CENTER, texFont, 3);


		//glScalef(2,2,1);

		if (position > 0x0600) {
			alphakitty += delta * 0.01;
			alphakitty = CLAMP(alphakitty, 0, 0.7);

			for (x = -128; x < 1024 + 256; x += 128) {

				i = (/*0.5f+0.5f**/sin(TWOPI * (x + ticks * 1.3) / 2000.0f)) * (BLUR_STEPS - 1);
				i = MAX(i, 0);
				//i=0;
				glBindTexture(GL_TEXTURE_2D, texBunny[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				Mode2D();
				glTranslatef(x + fmod(0*ticks*0.1, 128), 768 - 64, 0);
				glTranslatef(Sine(-64, 64, 2000, 0), 0, 0);
				glTranslatef(0, 16.0f*sin(TWOPI*(x + ticks*1.3) / 1000.0f), 0);
				//glRotatef(Sine(-10,10,3000,0),0,0,1);
				glBegin(GL_TRIANGLE_STRIP);

				glColor4f(1, 1, 1, alphakitty);
				glTexCoord2f(0, 0);
				glVertex2i(-128, -128);
				glTexCoord2f(1, 0);
				glVertex2i( 128, -128);

				glTexCoord2f(0, 1);
				glVertex2i(-128, 128);
				glTexCoord2f(1, 1);
				glVertex2i( 128, 128);
				glEnd();
			}
		}
		Mode2D();

		fallingPos += fallingSpeed * delta;
		fallingSpeed += delta;
		if (fallingPos > 384) {
			fallingPos = 384;
			fallingSpeed *= -0.5;
		}
		if ( fabs(fallingSpeed) < delta*2 ) {
			if (fallingSpeed < 0) {
				fallingSpeed = 0;
			}
		}
		glTranslatef(512, fallingPos, 0);
		glRotatef(ending*4, 0, 0, 1);
		DrawString(0, 0, "kawaii", TEXT_CENTER, texFont, 16 + 0.05*(ending*ending));


	}
	glEnable(GL_TEXTURE_2D);
}

void DrawDoomWipe(double time, double delta, int position)
{
	int x;

	partStart_Draw(time, delta, position, true);

	glBindTexture(GL_TEXTURE_2D, texRender);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, RENDER_SIZE, RENDER_SIZE, 0);
	Mode2D();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texRender);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	DrawTitle(time, delta, position);

	glBindTexture(GL_TEXTURE_2D, texRender);
	glBegin(GL_QUADS);
	bDoomwipe = false;
	for (x = 0; x < 1024; x += DOOMWIPE_SIZE) {
		float y, u, su;
		doomwipe[x] += frand() * delta * 12.0 + frand() * delta * 08.0 * (1.0 + 0.5 * CLAMP(sin(x * 3.1415927 * 2.0 / 2048.0), 0, 1));
		if (doomwipe[x] < 768) {
			bDoomwipe = true;
		}
		y = CLAMP(doomwipe[x], 0, 768);

		u  = x / 1024.0f;
		su = DOOMWIPE_SIZE / 1024.0f;

		glColor4f(1, 1, 1, 1.0 - y / 768.0 / 2.0);
		glTexCoord2f(u, 1);
		glVertex2i(x,  y);
		glTexCoord2f(u + su, 1);
		glVertex2i(x + DOOMWIPE_SIZE, y);
		glTexCoord2f(u + su, 0);
		glVertex2i(x + DOOMWIPE_SIZE, y + 768);
		glTexCoord2f(u, 0);
		glVertex2i(x,  y + 768);
	}
	glEnd();
}

void partTitle_Draw(double time, double delta, int position)
{
	bool wipe = true;

	if (bDoomwipe) {
		DrawDoomWipe(time, delta, position);
	} else {
		DrawTitle(time, delta, position);
	}

}

static void partTitle_Open()
{
	int i;

	glGenTextures(1, &texRainbow);
	StarsInit(stars);


	bmpBunny = LoadRLEBitmap(&bmp_bunny);

	for (i = 0; i < BLUR_STEPS; i++) {
		texBunny[i] = CreateTexture(bmpBunny->width, bmpBunny->height);

		glBindTexture(GL_TEXTURE_2D, texBunny[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		UploadTexture(texBunny[i], bmpBunny);
		Blur(bmpBunny, 2);
	}


}
static void partTitle_Close()
{
	FreeTexture(texRainbow);
	glDeleteLists(listStar, 1);
}



DemoPart partTitle = {0x0418, 0x0d00, partTitle_Draw, partTitle_Open, partTitle_Close};
