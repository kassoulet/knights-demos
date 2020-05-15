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

#define LETTERS_MAX 	800
#define LETTER_SIZE 	128
typedef struct Letter {
	float x, y, sx, sy;
	char letter;
	char order;
} Letter;

static Letter letters[LETTERS_MAX];

static float peek = 0;
static float morph = 0;
static float flash = 0;


void partStart_Draw(double time, double delta, int position, bool wipe)
{
	int i;
	float a;
	bool bMorph = false;

	int synchro[] = { 	0x0108, 0x0118, 0x0128, 0x0138,
	                   0x0208, 0x0218, 0x0228, 0x0238,
	                   0x0308, 0x0318, 0x0328, 0x0338,
	                   0x0408, 0x0418, 0x0420, 0x0428, 0x0430, 0x0438,
	                   0
	                };

	if (position >= 0x0300) {
		bMorph = true;
	}
	if (GetSynchro(synchro)) {
		peek = 20;
	}
	peek -= delta * 2.0;
	peek = MAX(0, peek);

	flash -= delta * 2.0;
	flash = MAX(0, flash);

	Mode2D();

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);

	if (flash > 0.0f) {
		glColor4f(1, 1, 1, 1);
		glVertex2i(0,   0);
		glVertex2i(1024, 0  );

		glColor4f(1, 1, 1, 1);
		glVertex2i(0,   768);
		glVertex2i(1024, 768);
	} else {
		glColor4f(0.5, 0.5, 0.5, 1);
		glVertex2i(0,   0);
		glVertex2i(1024, 0  );

		glColor4f(0.8, 0.8, 0.8, 1);
		glVertex2i(0,   768);
		glVertex2i(1024, 768);
	}
	glEnd();

	if (position >= 0x0100) {
		glEnable(GL_TEXTURE_2D);

		glTranslatef(512, 384, 0);
		glRotatef(Sine(-30, 30, 1000000, 0), 0, 0, 1);
		glTranslatef(-512, -384, 0);

		Mode2D();

		if (wipe) {
			glViewport(0, 0, RENDER_SIZE, RENDER_SIZE);
		}

		if (bMorph) {
			a = morph;
			morph += delta * 0.01;
			morph = CLAMP(morph, 0.0, 1.0);
		} else {
			a = 0.0f;
		}

		for (i = 0; i < LETTERS_MAX; i++) {
			char s[2];
			float x;
			letters[i].x += letters[i].sx * delta * 1.5f;
			letters[i].y += letters[i].sy * delta * 1.5f;
			if (letters[i].x > 1024 + LETTER_SIZE)  letters[i].x = -LETTER_SIZE;
			if (letters[i].y > 768 + LETTER_SIZE)   letters[i].y = -LETTER_SIZE;
			if (letters[i].x < -LETTER_SIZE)  letters[i].x = 1024 + LETTER_SIZE;
			if (letters[i].y < -LETTER_SIZE)  letters[i].y = 768 + LETTER_SIZE;
			s[0] = letters[i].letter;
			s[1] = 0;
			x = letters[i].x * (1.0f - a) + a * letters[i].order * 1024 / 8 + 1 * 1024 / 8;
			DrawString(	x + 40.0f*cos(letters[i].y*0.01f + timer*0.1f) + peek*(0.5 - frand()),
			            letters[i].y + 30.0f*sin(letters[i].x*0.01f + timer*0.1f) + peek*(0.5 - frand()),
			            s, TEXT_CENTER, texFont, 6);
		}
	}
}

static float doomwipe[1024];
#define DOOMWIPE_SIZE 2

void partStart_DrawInternal(double time, double delta, int position)
{
	bool wipe = false;
	partStart_Draw(time, delta, position, wipe);
}

static void partStart_Open()
{
	int i;
	for (i = 0; i < LETTERS_MAX; i++) {
		char message[] = "knights";
		letters[i].x = 1024 + rand() % 1024;
		letters[i].y = 768 + rand() % 768;
		letters[i].sx = 5.0f - frand() * 10.0f;
		letters[i].sy = 5.0f - frand() * 10.0f;
		letters[i].order = rand() % 7;
		letters[i].letter = message[letters[i].order];
	}

	for (i = 0; i < 1024; i++) {
		doomwipe[i] = -100;
	}

	texRender =  CreateTexture(RENDER_SIZE, RENDER_SIZE);
	glBindTexture(GL_TEXTURE_2D, texRender);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
static void partStart_Close()
{}



DemoPart partStart = {0x00000, 0x0418, partStart_DrawInternal, partStart_Open, partStart_Close};
