/* Kawaii - the intro - (c) 2004 Gautier Portet < kassoulet @ no-log . org >
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

/* to stop the song, if needed */
//int SONG_END = 0x0d00;
int SONG_END = 0xf500;

//int SONG_START = 0x19;
int SONG_START = 0x0;

extern DemoPart partStart;
extern DemoPart partTitle;
extern DemoPart partMetaballs;
extern DemoPart partKeftales;
extern DemoPart partCellular;

DemoPart* parts[] = {
	&partStart,
	&partTitle,
	&partMetaballs,
	//&partKeftales,
	&partCellular,
	NULL
};

GLuint texFont;
Surface *bmpFont;
Texture texRender;

void OpenParts()
{
	DemoPart **part = parts;
	char progress[32];
	int i = 0;
	while (*part) {
		glClearColor( 0.1, 0.2, 0.3, 1 );
		glClear( GL_COLOR_BUFFER_BIT );
		//DrawString(512, 384, "LOADING", TEXT_CENTER, texFont);
		progress[i] = '.';
		progress[i+1] = 0;
		i++;
		//DrawString(512, 400, progress, TEXT_CENTER, texFont);
		Flip();

		(*part++)->Open();
	}
}

void CloseParts()
{
	DemoPart **part = parts;
	while (*part) {
		fflush(stdout);
		(*part++)->Close();
	}
}

void DrawParts(double time, double delta, int position)
{
	DemoPart **ppart = parts;

	while (*ppart) {
		DemoPart *part = *ppart;
		if ( (position >= part->start) && (position < part->end) ) {
			part->Draw(time, delta, position);
		}
		ppart++;
	}
}

void InitNoise();
extern double oldTicks;

Surface *bmp;

typedef struct Resolution {
	int w, h;
} Resolution;

#define RESOLUTION_MAX 6
Resolution menu_resolution_list[] = { {320, 240}, {512, 384}, {640, 480}, {800, 600}, {1024, 768}, {1280, 1024}, {1600, 1200}};
int menu_resolution = 4;

typedef enum MENU_SELECTED {
	MENU_RESOLUTION,
	MENU_FULLSCREEN,
	MENU_START
};
int menu_selected = MENU_START;

bool menu_fullscreen = false;

#define STR_FULLSCREEN 	"Fullscreen"
#define STR_WINDOWED 	"Window"
#define STR_START	"Start"


int main(int argc, char** argv)
{
	bool intro_running = true;
	bool menu_running = true;
	Surface* bmp;
	Surface* bmp2;
	Surface* bmp3;
	GLuint	 tex;
	float z = 0;
	int i;

	/* Init */
	OpenSystem();
	OpenVideo(640, 480, 0, 0);

	glEnable(GL_TEXTURE_2D);

	/* Load font */
	bmpFont = LoadRLEBitmap(&bmp_font);

	/* add black border */
	AddBorder(bmpFont, 0xff000000);

	/* detect chars width */
	InitFont(bmpFont);

	/* and upload */
	texFont = CreateTexture(256, 128);
	glBindTexture(GL_TEXTURE_2D, texFont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	UploadTexture(texFont, bmpFont);

	while (menu_running) {
		KEY key;
		char str[128], str2[128];
		char* str_normal = "%s";
		char* str_selected = "[ %s ]";
		char* str_used;

		glClearColor( 0.1, 0.2, 0.3, 1 );
		glClear( GL_COLOR_BUFFER_BIT );
		Mode2D();

		sprintf(str2, "%dx%d", menu_resolution_list[menu_resolution].w, menu_resolution_list[menu_resolution].h);
		if (menu_selected == MENU_RESOLUTION) {
			str_used = str_selected;
		} else {
			str_used = str_normal;
		}
		sprintf(str, str_used, str2);
		DrawString(512, 304, str, TEXT_CENTER, texFont, 4);

		if (menu_selected == MENU_FULLSCREEN) {
			str_used = str_selected;
		} else {
			str_used = str_normal;
		}
		sprintf(str, str_used, menu_fullscreen ? STR_FULLSCREEN : STR_WINDOWED);
		DrawString(512, 384, str, TEXT_CENTER, texFont, 4);

		if (menu_selected == MENU_START) {
			str_used = str_selected;
		} else {
			str_used = str_normal;
		}
		sprintf(str, str_used, STR_START);
		DrawString(512, 464, str, TEXT_CENTER, texFont, 4);

		DrawString(512, 80, "Knights Sexy Menu", TEXT_CENTER, texFont, 6);

		Flip();

		key = GetKeyPressed();
		switch (key) {
		case KEY_ESCAPE:
			menu_running = false;
			intro_running = false;
			break;
		case KEY_UP:
			menu_selected--;
			break;
		case KEY_DOWN:
			menu_selected++;
			break;
		case KEY_LEFT:
			if (menu_selected == MENU_FULLSCREEN) {
				menu_fullscreen = !menu_fullscreen;
			} else if (menu_selected == MENU_RESOLUTION) {
				menu_resolution--;
			}
			break;
		case KEY_RIGHT:
			if (menu_selected == MENU_FULLSCREEN) {
				menu_fullscreen = !menu_fullscreen;
			} else if (menu_selected == MENU_RESOLUTION) {
				menu_resolution++;
			}
			break;
		case KEY_RETURN:
			if (menu_selected == MENU_FULLSCREEN) {
				menu_fullscreen = !menu_fullscreen;
			} else if (menu_selected == MENU_START) {
				menu_running = false;
			}
			break;
		}
		menu_selected = CLAMP(menu_selected, 0, MENU_START);
		menu_resolution = CLAMP(menu_resolution, 0, RESOLUTION_MAX);

	}
	FreeTexture(texFont);

	if (!intro_running) {
		FreeSurface(bmpFont);
		CloseVideo();
		CloseSystem();
		return 0;
	}

	OpenVideo(menu_resolution_list[menu_resolution].w, menu_resolution_list[menu_resolution].h, menu_fullscreen, 4);

	/* re-upload font */
	texFont = CreateTexture(256, 128);
	glBindTexture(GL_TEXTURE_2D, texFont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	UploadTexture(texFont, bmpFont);

	/* for render to texture */
	texRender =  CreateTexture(RENDER_SIZE, RENDER_SIZE);
	glBindTexture(GL_TEXTURE_2D, texRender);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	//DrawString(512, 384, "", TEXT_CENTER, texFont);
	//Flip();

	OpenParts();
	OpenMusic(data_module, sizeof(data_module));

	/* Skip pattern, if needed */
	if (SONG_START) {
		SetMusicPosition(SONG_START);
	}

	ticks = 0;
	oldTicks = 0;
	delta = 0;

	while (intro_running) {

		int position;
		int synchro;
		float a1, a2;

		position = GetMusicPosition();

		if (CheckUserBreak()) {
			intro_running = false;
		}
		if (position >= SONG_END) {
			intro_running = false;
		}

		DrawParts(ticks, delta, position);

		if (ticks) {
			char str[256];
			Mode2D();
			sprintf(str, "%.1f fps  %04x", 1000.0 / ms, position);
			DrawString(2, 2, str, TEXT_LEFT, texFont, 2);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			int i, x = 10, y = 768 - 10, w = 256, h = 128;

			glBegin(GL_TRIANGLE_STRIP);
			glColor4f(0, 0, 0, 0.5);
			glVertex2f(x,     y - h);
			glVertex2f(x + w, y - h);
			glVertex2f(x,     y);
			glVertex2f(x + w, y);
			glEnd();

			for (i = 0; i < MS_SIZE; i++) {
				float sx = (float) w / MS_SIZE;
				float sy = (float) h / 30;
				float px = i * sx;
				float py = ms_array[i] * sy;

				glBegin(GL_TRIANGLE_STRIP);
				glColor4f(1, 1, 1, 0.5);
				glVertex2f(x + px,      y - py);
				glVertex2f(x + px + sx, y - py);
				glVertex2f(x + px,      y);
				glVertex2f(x + px + sx, y);
				glEnd();
			}
		}


		Flip();
	}

	printf("%d fps\n", (int) (1000*frames / ticks));

	CloseMusic();

	FreeTexture(texFont);
	FreeSurface(bmpFont);

	CloseParts();
	CloseVideo();
	CloseSystem();
	/*
		LoadRLEBitmap(&bmp_font);
		LoadRLEBitmap(&bmp_kitty);
		LoadRLEBitmap(&bmp_babies);
		LoadRLEBitmap(&bmp_k1);
		LoadRLEBitmap(&bmp_k2);
		LoadRLEBitmap(&bmp_k3);
		LoadRLEBitmap(&bmp_back);
		LoadRLEBitmap(&bmp_greets);
	*/
	return 0;
}

/*
static Color createHue( double h ) {
        h *= 6.0;
        int hi = static_cast<int>( h );
        double hf = h - hi;


        switch( hi % 6 ) {
            case 0:
            return Color( 1.0 , hf, 0.0 );
            case 1:
            return Color( 1.0 - hf, 1.0, 0.0 );
            case 2:
            return Color( 0.0 , 1.0, hf );
            case 3:
            return Color( 0.0, 1.0 - hf, 1.0 );
            case 4:
            return Color( hf, 0.0, 1.0 );
            case 5:
            return Color( 1.0, 0.0, 1.0 - hf );
        }

        return Color();
    }

	bmp = LoadRLEBitmap(&bmp_back);
        bmp2 = CreateSurface(512,256,4);

	Noisify(bmp,20);
	gluScaleImage(GL_RGBA,256,128,GL_UNSIGNED_BYTE,bmp->pixels,512,256,GL_UNSIGNED_BYTE,bmp2->pixels);

        bmp3 = CreateSurface(512*2,256*2,4);
	Noisify(bmp2,10);
	gluScaleImage(GL_RGBA,512,256,GL_UNSIGNED_BYTE,bmp2->pixels,512*2,256*2,GL_UNSIGNED_BYTE,bmp3->pixels);
	Noisify(bmp3,5);

        tex = CreateTexture(512*2,256*2);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	UploadTexture(tex, bmp3);
*/
