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

/*
	Voxel Metaballs.
	don't even ask how it works, I don't know :]
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "introsystem.h"

/* maximal size of a grid dimension */
#define GRID_MAX 128

/* number of balls */
#define BALLS_NUM 6

int grid_size;
float fgrid_size;
float zoom_factor;
int potential = 800;
float potential_delta;

/* the display list */
static GLuint listCube = 0;

/* for profiling: how many voxel computed and drawn */
static int voxel_drawn = 0;
static int voxel_computed = 0;
static int voxel_frames = 0;

static Surface* bmpTexture;
static Texture  texTexture;

/*  */
typedef struct Ball {
	float x, y, z;
	float sx, sy, sz;
} Ball;
static Ball balls[BALLS_NUM];

/* lookup: is the voxel already computed? */
static Uint8 computed_buffer[GRID_MAX*GRID_MAX*GRID_MAX/8];

/* dirty buffer */
#define DIRTY_SIZE 0x10000
static Uint32 dirty_buffer[DIRTY_SIZE];
static int dirty_index = 0;

static float intro_alpha = 0;

inline Uint32 MakeIndex(Uint8 x, Uint8 y, Uint8 z)
{
	return x*GRID_MAX*GRID_MAX + y*GRID_MAX + z;
}

inline Uint8 IsComputed(Uint32 index)
{
	return computed_buffer[index/8] & (1 << (index&7));
}

inline void MarkComputed(Uint32 index)
{
	computed_buffer[index/8] |= 1 << (index & 7);
}

// TODO: we just need to clear 8 at a time, no need for an if !
inline void ClearComputed(Uint32 index)
{
	if (computed_buffer[index/8]) {
		computed_buffer[index/8] = 0;
	}
}

void ClearDirtyBuffer()
{
	while (dirty_index > 0) {
		dirty_index--;
		ClearComputed(dirty_buffer[dirty_index]);
	}
	dirty_index = 0;
}

inline void AddToDirtyBuffer(Uint32 index)
{
	/* this voxel will need to be cleared */
	dirty_buffer[dirty_index] = index;
	dirty_index++;
}

/* lookup: convert a Uint8 to its real position */
float lut_position_to_float[GRID_MAX];

/* lookup: convert a Uint8 to its unscaled position */
float lut_position_to_unscaled[GRID_MAX];

inline void DrawCube(Uint8 x, Uint8 y, Uint8 z)
{
	glLoadIdentity();
	glTranslatef(lut_position_to_float[x], lut_position_to_float[y], lut_position_to_float[z]);
	glTexCoord2f(lut_position_to_unscaled[x], lut_position_to_unscaled[y]);

	glCallList(listCube);
	voxel_drawn++;
}


float TestDist(float x)
{
	int i;
	float e = 0;
	float dist;

	x = x / fgrid_size;

	for (i = 0; i < BALLS_NUM; i++) {
		dist = 3.0f * x * x;

		dist = ((float)BALLS_NUM * BALLS_NUM) / dist * 0.1;
		e += dist * 1.0;
	}
	return e;
}


inline int ComputeEnergy(int x, int y, int z)
{
	int i;
	float e = 0;
	float dist;
	float fx, fy, fz;

	fx = lut_position_to_unscaled[x];
	fy = lut_position_to_unscaled[y];
	fz = lut_position_to_unscaled[z];

	for (i = 0; i < BALLS_NUM; i++) {
		dist = (balls[i].x - fx) * (balls[i].x - fx) +
		       (balls[i].y - fy) * (balls[i].y - fy) +
		       (balls[i].z - fz) * (balls[i].z - fz);

		dist = ((float)BALLS_NUM * BALLS_NUM) / dist * 0.1f;
		e += dist;
	}

	voxel_computed++;
	if ( (e > potential - potential_delta) && (e < potential + potential_delta) ) {
		return 1;
	}
	return 0;
}



void AddVoxel(int x, int y, int z)
{
	int e;
	Uint32 index;

	/* test for any of xyz being >GRID_MAX *AND* <0 */
	if ( (Uint8)(x | y | z) >= GRID_MAX ) {
		return;
	}

	index = MakeIndex(x, y, z);

	/* we already have done it */
	if (IsComputed(index)) {
		return;
	}

	/* mark it */
	e = ComputeEnergy(x, y, z);
	MarkComputed(index);
	AddToDirtyBuffer(index);

	if ( e > 0 ) {
		/* we only draw if we are at an energy border */
		DrawCube(x, y, z);

		AddVoxel(x - 1, y, z);
		AddVoxel(x + 1, y, z);
		AddVoxel(x, y - 1, z);
		AddVoxel(x, y + 1, z);
		AddVoxel(x, y, z - 1);
		AddVoxel(x, y, z + 1);
	}
}

void GenCube_color()
{
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0, 0.2f);
	glVertex3f( 0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f( 0.5f, 0.5f, 0.5f);

	glVertex3f( 0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f( 0.5f, -0.5f, -0.5f);

	glVertex3f( 0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f( 0.5f, -0.5f, 0.5f);

	glColor3f(1, 0.5f, 1);
	glVertex3f( 0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f( 0.5f, 0.5f, -0.5f);

	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glVertex3f( 0.5f, 0.5f, -0.5f);
	glVertex3f( 0.5f, 0.5f, 0.5f);
	glVertex3f( 0.5f, -0.5f, 0.5f);
	glVertex3f( 0.5f, -0.5f, -0.5f);

	glEnd();
}

void partMetaballs_Draw(double time, double delta, int position)
{
	float f;
	int i;

	/* intro fade */
	intro_alpha += delta * 0.01f;
	intro_alpha = CLAMP(intro_alpha, 0, 1);

	/* size of grid */
	fgrid_size = Sine(16, intro_alpha * GRID_MAX, 10000, 0);
	zoom_factor = GRID_MAX / fgrid_size;
	grid_size = fgrid_size;

	/* try to guess the size of the potential border (HACK) */
	for (f = 0; f < fgrid_size; f += 0.1) {
		float e;
		e = TestDist(f);
		if (e < potential) {
			potential_delta = 0.9 * (e - TestDist(f + 1.0));
			break;
		}
	}

	/* create the displaylist if needed */
	if (listCube == 0) {
		listCube = glGenLists(1);
		glNewList(listCube, GL_COMPILE);
		GenCube_color();
		glEndList();
	}

	/* create our tables */
	for (i = 0; i < GRID_MAX; i++) {
		lut_position_to_float[i] = (float) i - fgrid_size / 2.0f;
		lut_position_to_unscaled[i] = (float) i / fgrid_size;
	}

	/* clear screen and/or zbuffer */
	if (intro_alpha < 1.0f) {
		glClearColor( 1, 1, 1, 1 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable(GL_BLEND);
	} else {
		glClear( GL_DEPTH_BUFFER_BIT );
		glDisable(GL_BLEND);
	}

	ClearDirtyBuffer();

	/* draw background */
	Mode2D();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glLoadIdentity();
	glTranslatef(512, 384, 0);
	glBegin(GL_TRIANGLE_STRIP);

	glColor4f(1, 1, 1, intro_alpha);
	glTexCoord2f(0, 0);
	glVertex2i(-512, -384);
	glTexCoord2f(1, 0);
	glVertex2i( 512, -384);

	glTexCoord2f(0, 0.75);
	glVertex2i(-512, 384);
	glTexCoord2f(1, 0.75);
	glVertex2i( 512, 384);
	glEnd();

	/* place camera */
	Mode2D();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)ScreenWidth / (GLfloat)ScreenHeight, 0.1f, 2000.0f);
	glScalef(1, intro_alpha, 1);
	glTranslatef(0, 0, -120);
	glScalef(zoom_factor, zoom_factor, zoom_factor);
	glRotatef(20, 1, 0, 0);
	glRotatef(timer*0.21, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	/* place balls */
	for (i = 0; i < BALLS_NUM; i++) {
		balls[i].x = (0.5f + 0.2f * sin(TWOPI * (16 * i + 1 * timer * 0.21) / 100.0));
		balls[i].y = (0.5f + 0.2f * sin(TWOPI * (27 * i + 1 * timer * 0.42) / 120.0));
		balls[i].z = (0.5f + 0.2f * sin(TWOPI * (18 * i + 1 * timer * 0.33) / 140.0));
	}

	/* and draw them */
	for (i = 0; i < BALLS_NUM; i++) {
		Uint8 x, y, z;

		x = balls[i].x * fgrid_size;
		y = balls[i].y * fgrid_size;
		z = balls[i].z * fgrid_size;

		/* we start from the center of the ball, and we move along y axis,
		 * until we find an energy border to start the 3d floodfill */

		while (1) {
			/* find border */
			if (ComputeEnergy(x, y, z) > 0) {
				if (!IsComputed(MakeIndex(x, y, z))) {
					/* floodfill from this position */
					AddVoxel(x, y, z);
				}
				break;
			} else {
				/* continue moving */
				y--;
				if (y <= 0) break;
			}
		}
	}
	voxel_frames++;
}


static void partMetaballs_Open()
{
	Surface *tmp[4];
	int i;

	memset(computed_buffer, 0, sizeof(computed_buffer));

	tmp[0] = LoadRLEBitmap(&bmp_back);

	for (i = 0; i < 256*128; i++) {
		/* set all alpha to 255 */
		tmp[0]->pixels[4*i+3] = 255;
	}

	tmp[1] = CreateSurface(256, 256, 4);
	memset(tmp[1]->pixels, 187, 256*256*4);
	memcpy(tmp[1]->pixels + 32*256*4, tmp[0]->pixels, 256*128*4);

	Noisify(tmp[1], 16);
	tmp[2] = Scale(tmp[1], 2);
	Noisify(tmp[2], 8);
	tmp[3] = Scale(tmp[2], 2);
	Noisify(tmp[3], 4);

	/* and upload */
	texTexture = CreateTexture(256, 256);
	glBindTexture(GL_TEXTURE_2D, texTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	UploadTextureMipmap(texTexture, tmp[3]);

	for (i = 0; i < 4; i++) {
		FreeSurface(tmp[i]);
	}
}



static void partMetaballs_Close()
{
	glDeleteLists(listCube, 1);

	FreeTexture(texTexture);
	if (voxel_frames) {
		printf("Metaballs - voxel computed: %d/frame - voxel drawn: %d/frame\n",
			   voxel_computed / voxel_frames, voxel_drawn / voxel_frames);
	}
}



DemoPart partMetaballs = {0x0d00, 0x1900, partMetaballs_Draw, partMetaballs_Open, partMetaballs_Close};


