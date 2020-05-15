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


float sfrand( int *seed )
{
	float res;

	seed[0] *= 16807;

	*((unsigned int *) &res) = ( ((unsigned int)seed[0]) >> 9 ) | 0x40000000;

	return( res - 3.0f );
}

typedef struct Vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Vertex;

typedef struct Color {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} Color;


void partCellular_Draw(double time, double delta, int position)
{
	float f;

	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Mode2D();
	//glTranslatef(512,384,0);
	//glScalef(2,2,0);

	// set 2d mode
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_CULL_FACE);

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	//glRotatef(mx/1.0,1,0,0);

	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1, 1, 1, 1);
	glVertex3f(0, 0, -1);
	glVertex3f(1, 0, -1);
	glVertex3f(0, 1, 0);
	glVertex3f(1, 1, 0);
	glEnd();

	glClear( GL_COLOR_BUFFER_BIT);*/

	//mx = 2;
	my = 0;

	// render the cells
	const int numCells = 200+0*mx*10;
	int sem = 134;
	int segments = 48;
	int i, j;
	srand(1234);

	segments = 3 + my / 10;
	Vertex *vertexarray;
	Color  *colorarray;
	int vertices = 3 * segments * numCells;
	vertexarray = (GLfloat*) malloc(vertices * sizeof(Vertex));
	colorarray =  (GLfloat*) malloc(vertices * sizeof(Color));

	int vertex = 0;
	for ( j = 0; j < numCells; j++ ) {
		float x = cosf( sfrand(&sem) * 3.14f + sfrand(&sem) * time * 0.000 );
		float y = cosf( sfrand(&sem) * 3.14f + sfrand(&sem) * time * 0.000 );
		//x *=  cosf(sfrand(&sem) * time * 0.001 * i * 0.01 );
		//y *=  cosf(sfrand(&sem) * time * 0.001 * i * 0.01 );
		float z = 0;
		float angle = 0;

		//x = 1*cosf(rand() + time * 0.0);
		//y = 1*cosf(rand() + time * 0.0);

		angle += 0*j*0.05 + time*0.0001 + 0*cosf(time * 0.0003);

		x += 0.1 * cosf(time * 0.0013);
		y += 0.1 * sinf(time * 0.0017);
		x += 0.1 * cosf(time * 0.001 + j);
		y += 0.1 * sinf(time * 0.001 + j);

		//z += 0.1 * sinf(time * 0.001 + j);

		float size = 1.0;
		//size += 0.5 * sinf(time * 0.001 + j);

		float r=0.5+0.5*x,g=0.5+0.5*y,b=0.5+0.5*y;
		float a = 1 + 0*0.5;
		for ( i = 0; i < segments; i++ ) {
			float an = (6.28318f / (segments - 0)) * (float)i;
			an += angle;
			vertexarray[vertex].x = x + size * cosf(an);
			vertexarray[vertex].y = y + size * sinf(an);
			vertexarray[vertex].z = z + 1.0f;

			float s = 1.0 * i / segments;
			colorarray[vertex].r = r*s;
			colorarray[vertex].g = g*s;
			colorarray[vertex].b = b*s;
			colorarray[vertex].a = a;
			vertex++;

			vertexarray[vertex].x = x;
			vertexarray[vertex].y = y;
			vertexarray[vertex].z = z - 1.0f;

			colorarray[vertex].r = r*s;
			colorarray[vertex].g = g*s;
			colorarray[vertex].b = b*s;
			colorarray[vertex].a = a;
			vertex++;

			an = (6.28318f / (segments - 0)) * (float)(i + 1);
			an += angle;
			vertexarray[vertex].x = x + size * cosf(an);
			vertexarray[vertex].y = y + size * sinf(an);
			vertexarray[vertex].z = z + 1.0f;

			colorarray[vertex].r = r*s;
			colorarray[vertex].g = g*s;
			colorarray[vertex].b = b*s;
			colorarray[vertex].a = a;
			vertex++;
		}
	}
	glVertexPointer(3, GL_FLOAT, 0, vertexarray);
	glColorPointer(4, GL_FLOAT, 0, colorarray);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



	free(vertexarray);
}

void partCellular_Draw_(double time, double delta, int position)
{
	float f;

	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Mode2D();
	//glTranslatef(512,384,0);
	//glScalef(2,2,0);

	// set 2d mode
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	//glRotatef(mx/1.0,1,0,0);

	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);

	mx = 20;
	my = 0;

	// render the cells
	const int numCells = mx*10;
	int sem = 134;
	int segments = 48;
	int i, j;
	srand(1234);

	segments = 4 + my / 10;
	//segments = 4;


	for ( j = 0; j < numCells; j++ ) {
		// move the cell on the screen
		float x = cosf( sfrand(&sem) * 3.14f + sfrand(&sem) * time * 0 );
		float y = cosf( sfrand(&sem) * 3.14f + sfrand(&sem) * time * 0 );
		float z = 0;

		x = cosf(rand() + time * 0.0);
		y = cosf(rand() + time * 0.0);

		x += 0.1 * cosf(time * 0.0013);
		y += 0.1 * sinf(time * 0.0017);

		//z += 1.0+0.1*sinf(x+y+time*0.0017);

		//printf("%d %.2f %.2f\n", j, x, y);

		// render cone (can be optimized of course)
		glBegin( GL_TRIANGLE_FAN );
		//glColor4f( x, x, y, 1.0f );
		float r=0.5+0.5*x,g=0.5+0.5*y,b=0.5+0.5*y;
		glColor4f( r, g, b, 1.0f );
		//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glVertex3f( x, y, z - 1.0f );
		//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		for ( i = 0; i < segments; i++ ) {
			float an = (6.28318f / (segments - 1)) * (float)i;
			an += cosf(time * 0.0003);
			float s = 1.0 * i / segments;
			glColor4f( r*s, g*s, b*s, 1.0f );
			glVertex3f( x + cosf(an), y + sinf(an), z + 1.0f );
		}
		glEnd();
	}
}

void partCellular_Draw___(double time, double delta, int position)
{
	float f;

	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Mode2D();
	//glTranslatef(512,384,0);
	//glScalef(2,2,0);

	// set 2d mode
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	//glRotatef(mx/1.0,1,0,0);

	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);

	mx = 2;
	my = 0;

	// render the cells
	const int numCells = mx*10;
	int sem = 134;
	int segments = 48;
	int i, j;
	srand(1234);


	segments = 4 + my / 10;
	//segments = 4;


	for ( j = 0; j < numCells; j++ ) {
		// move the cell on the screen
		float x = cosf( sfrand(&sem) * 3.14f + sfrand(&sem) * time * 0 );
		float y = cosf( sfrand(&sem) * 3.14f + sfrand(&sem) * time * 0 );
		float z = 0;

		x = cosf(rand() + time * 0.0);
		y = cosf(rand() + time * 0.0);

		x += 0.1 * cosf(time * 0.0013);
		y += 0.1 * sinf(time * 0.0017);

		//z += 1.0+0.1*sinf(x+y+time*0.0017);

		//printf("%d %.2f %.2f\n", j, x, y);

		// render cone (can be optimized of course)
		glBegin( GL_TRIANGLES );
		//glColor4f( x, x, y, 1.0f );
		float r=0.5+0.5*x,g=0.5+0.5*y,b=0.5+0.5*y;
		glColor4f( r, g, b, 1.0f );
		glVertex3f( x, y, z - 1.0f );
		for ( i = 0; i < segments; i++ ) {
			float an = (6.28318f / (segments - 1)) * (float)i;
			glColor4f( b, g, r, 1.0f );
			glVertex3f( x + cosf(an), y + sinf(an), z + 1.0f );
			glColor4f( r, g, b, 1.0f );
			glVertex3f( x, y, z - 1.0f );
			an = (6.28318f / (segments - 1)) * (float)(i-1);
			glColor4f( b, r, g, 1.0f );
			glVertex3f( x + cosf(an), y + sinf(an), z + 1.0f );
		}
		glEnd();
	}
}

static void partCellular_Open()
{
}
static void partCellular_Close()
{
}



DemoPart partCellular = {0x1900, 0xfd00, partCellular_Draw, partCellular_Open, partCellular_Close};


