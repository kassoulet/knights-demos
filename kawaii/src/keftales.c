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

#define RENDER_SIZE 512

Surface* bmpBabies;
Texture  texBabies;

Surface* bmpNoise;
Texture  texNoise;

Texture  texCube;

#define NOISE_SIZE 256

void GenCube_texture()
{
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f( 0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 1);
	glVertex3f( 0.5f, 0.5f, 0.5f);

	glTexCoord2f(1, 1);
	glVertex3f( 0.5f, -0.5f, 0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 0);
	glVertex3f( 0.5f, -0.5f, -0.5f);

	glTexCoord2f(1, 1);
	glVertex3f( 0.5f, 0.5f, 0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 0);
	glVertex3f( 0.5f, -0.5f, 0.5f);

	glTexCoord2f(1, 0);
	glVertex3f( 0.5f, -0.5f, -0.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(1, 1);
	glVertex3f( 0.5f, 0.5f, -0.5f);

	glTexCoord2f(1, 1);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 0);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glTexCoord2f(1, 0);
	glVertex3f( 0.5f, 0.5f, -0.5f);
	glTexCoord2f(1, 1);
	glVertex3f( 0.5f, 0.5f, 0.5f);
	glTexCoord2f(0, 1);
	glVertex3f( 0.5f, -0.5f, 0.5f);
	glTexCoord2f(0, 0);
	glVertex3f( 0.5f, -0.5f, -0.5f);

	glEnd();
}


void partKeftales_Draw_(double time, double delta, int position)
{
	float f;

	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	glEnable(GL_TEXTURE_2D);
	glViewport(0, 0, RENDER_SIZE, RENDER_SIZE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, RENDER_SIZE, RENDER_SIZE, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Feedback */
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, texRender);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTranslatef(RENDER_SIZE / 2, RENDER_SIZE / 2, 0);
	//glRotatef(Sine(-50,50,60000,300), 0,0,1);
	glTranslatef(Sine(-1, 1, 800, 0)*0.9, Sine(-1, 1, 800, 200)*0.9, 0);
	glRotatef(Sine(-1, 1, 20000, 0)*2.0, 0, 0, 1);
	glScalef(1.01 + 0.02*Sine(-1, 1, 10000, 0), 1.01 + 0.02*Sine(-1, 1, 10000, 0), 1);
	//glScalef(1.41,1.41,1);
	glTranslatef(frand() - 0.5, frand()*1.0 - 0.5, 0);

	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1, 1, 1, 1);
	glTexCoord2f(0, 1);
	glVertex2i(-RENDER_SIZE / 2, -RENDER_SIZE / 2);
	glTexCoord2f(1, 1);
	glVertex2i( RENDER_SIZE / 2, -RENDER_SIZE / 2);
	glTexCoord2f(0, 0);
	glVertex2i(-RENDER_SIZE / 2, RENDER_SIZE / 2);
	glTexCoord2f(1, 0);
	glVertex2i( RENDER_SIZE / 2, RENDER_SIZE / 2);
	glEnd();

	/* Noise */

	glBindTexture(GL_TEXTURE_2D, texNoise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glRotatef(rand(), 0, 0, 1);

	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1, 1, 1, 0.1);
	glTexCoord2f(0, 0);
	glVertex2i( 0, 0);
	glTexCoord2f(1, 0);
	glVertex2i( RENDER_SIZE, 0);
	glTexCoord2f(0, 1);
	glVertex2i( 0, RENDER_SIZE);
	glTexCoord2f(1, 1);
	glVertex2i( RENDER_SIZE, RENDER_SIZE);
	glEnd();

	/* Sprite */
	glBindTexture(GL_TEXTURE_2D, texBabies);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glLoadIdentity();
	glTranslatef(RENDER_SIZE / 2, RENDER_SIZE / 2, 0);
	//glTranslatef(Sine(-5,5,4000,0),Sine(-5,5,3000,10),0);
	glScalef(0.2 + Sine(-0.1, 0.1, 50000, 0), 0.2 + Sine(-0.1, 0.1, 50000, 0), 1);
	glRotatef(Sine(-50, 50, 30000, 0), 0, 0, 1);
	glRotatef(-time*0.01, 0, 0, 1);

	glEnable(GL_BLEND);

	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1, 1, 1, 1);
	glTexCoord2f(0, 0);
	glVertex2i(-RENDER_SIZE / 2, -RENDER_SIZE / 2);
	glTexCoord2f(1, 0);
	glVertex2i( RENDER_SIZE / 2, -RENDER_SIZE / 2);
	glTexCoord2f(0, 1);
	glVertex2i(-RENDER_SIZE / 2, RENDER_SIZE / 2);
	glTexCoord2f(1, 1);
	glVertex2i( RENDER_SIZE / 2, RENDER_SIZE / 2);
	glEnd();

	glDisable(GL_BLEND);



	glBindTexture(GL_TEXTURE_2D, texRender);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, RENDER_SIZE, RENDER_SIZE, 0);

	/* Draw the texture */
	glClearColor(0.6, 0.5, 0.4, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	Mode2D();

	glBindTexture(GL_TEXTURE_2D, texRender);
	glBindTexture(GL_TEXTURE_2D, texBabies);
	glTranslatef(512, 384, 0);
	glRotatef(time*0.01, 0, 0, 1);
	glScalef(2, 2, 0);

	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1, 1, 1, 1);
	glTexCoord2f(0, 1);
	glVertex2i(-256, -256);
	glTexCoord2f(1, 1);
	glVertex2i( 256, -256);
	glTexCoord2f(0, 0);
	glVertex2i(-256, 256);
	glTexCoord2f(1, 0);
	glVertex2i( 256, 256);
	glEnd();
}

void partKeftales_Draw(double time, double delta, int position)
{
	float f;

	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Sprite */
	glBindTexture(GL_TEXTURE_2D, texBabies);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glLoadIdentity();
	glTranslatef(RENDER_SIZE / 2, RENDER_SIZE / 2, 0);
	glTranslatef(Sine(-5, 5, 4000, 0), Sine(-5, 5, 3000, 10), 0);
	glScalef(0.2 + Sine(-0.1, 0.1, 50000, 0), 0.2 + Sine(-0.1, 0.1, 50000, 0), 1);
	glRotatef(Sine(-50, 50, 30000, 0), 0, 0, 1);
	glRotatef(-time*0.01, 0, 0, 1);

	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, texBabies);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Mode2D();
	glTranslatef(512, 384, 0);
	glScalef(2, 2, 0);


	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1, 1, 1, 1);
	glTexCoord2f(0, 0);
	glVertex2i(-256, -256);
	glTexCoord2f(2, 0);
	glVertex2i( 256, -256);
	glTexCoord2f(0, 2);
	glVertex2i(-256, 256);
	glTexCoord2f(2, 2);
	glVertex2i( 256, 256);
	glEnd();
}


static void partKeftales_Open()
{
	bmpBabies = LoadRLEBitmap(&bmp_greets);

	texBabies = CreateTexture(bmpBabies->width, bmpBabies->height);

	glBindTexture(GL_TEXTURE_2D, texBabies);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	UploadTexture(texBabies, bmpBabies);

	bmpNoise = CreateSurface(NOISE_SIZE, NOISE_SIZE, 4);
	Noisify(bmpNoise, 200);
	texNoise = CreateTexture(NOISE_SIZE, NOISE_SIZE);
	glBindTexture(GL_TEXTURE_2D, texNoise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	UploadTexture(texNoise, bmpNoise);

	texCube = CreateTexture(RENDER_SIZE, RENDER_SIZE);
}
static void partKeftales_Close()
{
	FreeTexture(texBabies);
	FreeSurface(bmpBabies);
	FreeTexture(texNoise);
	FreeSurface(bmpNoise);
	FreeTexture(texCube);
}



DemoPart partKeftales = {0x00, 0xfd00, partKeftales_Draw, partKeftales_Open, partKeftales_Close};
