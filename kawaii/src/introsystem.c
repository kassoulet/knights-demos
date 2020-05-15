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


#ifndef USE_SDL
/* win32 specific stuff */
#include <windows.h>
HWND hWnd = 0;
HDC hDC;
HGLRC hRC;
#endif

#include "introsystem.h"
int ScreenWidth = 640;
int ScreenHeight = 480;
int BufferWidth = 512;
int BufferHeight = 384;

int LowDetails = 0;
int Fullscreen = 0;

Uint32 ticks = 0;
double delta;
Uint32 frames = 0;
double timer = 0;
double oldTicks = 0;
double ms = 0;

/***************************************************************************/

int stat_textures = 0;
int stat_texture_mem = 0;
int stat_surfaces = 0;
int stat_surface_mem = 0;


void OpenSystem()
{
#ifdef USE_SDL
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit( 1 );
	}
	putenv("SDL_VIDEO_CENTERED=1");
#else
	/* 1 ms timer precision */
	timeBeginPeriod(1);
#endif
}

void CloseSystem()
{
#ifdef USE_SDL
	SDL_Quit();
#else
	printf("CloseSystem\n");
	fflush(stdout);

	timeEndPeriod(1);
#endif
	printf("%d textures used: %dKb\n", stat_textures, stat_texture_mem);
	printf("%d surfaces used: %dKb\n", stat_surfaces, stat_surface_mem);
}

int CheckUserBreak()
{
#ifdef USE_SDL
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			return 1;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				return 1;
			}
		}
	}
#else
	if (GetAsyncKeyState(VK_ESCAPE)&0xf000) {
		return 1;
	}
#endif
	return 0;
}

#ifndef USE_SDL
KEY last_key = KEY_NONE;
#endif

KEY GetKeyPressed()
{
#ifdef USE_SDL
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			return KEY_ESCAPE;
		}
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return KEY_ESCAPE;
				break;
			case SDLK_LEFT:
				return KEY_LEFT;
				break;
			case SDLK_RIGHT:
				return KEY_RIGHT;
				break;
			case SDLK_UP:
				return KEY_UP;
				break;
			case SDLK_DOWN:
				return KEY_DOWN;
				break;
			case SDLK_RETURN:
				return KEY_RETURN;
				break;
			default:
				break;
			}
		}
	}
#else
	MSG msg;
	while ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ) {
		if ( GetMessage(&msg, NULL, 0, 0) > 0 ) {
			DispatchMessage(&msg);
		}
	}

	if (last_key != KEY_NONE) {
		KEY key;
		key = last_key;
		last_key = KEY_NONE;
		return key;
	}

#endif
	return KEY_NONE;
}


/***************************************************************************/

#ifndef USE_SDL

int main(int argc, char**argv);

int WINAPI WinMain (HINSTANCE h, HINSTANCE p, LPSTR c, int m)
{
	main(0, NULL);
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{
	switch ( message ) {
	case WM_CLOSE:
		DestroyWindow( hWnd );
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SYSCOMMAND: {
		if ((wParam&0xFFF0) == SC_SCREENSAVE ||
		        (wParam&0xFFF0) == SC_MONITORPOWER)
			return 0;
	}

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			last_key = KEY_ESCAPE;
			break;
		case VK_LEFT:
			last_key = KEY_LEFT;
			break;
		case VK_RIGHT:
			last_key = KEY_RIGHT;
			break;
		case VK_UP:
			last_key = KEY_UP;
			break;
		case VK_DOWN:
			last_key = KEY_DOWN;
			break;
		case VK_RETURN:
			last_key = KEY_RETURN;
			break;
		}
		break;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);
}

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;

	/* get the device context */
	*hDC = GetDC (hWnd);

	/* set the pixel format for the DC */
	ZeroMemory (&pfd, sizeof (pfd));
	pfd.nSize = sizeof (pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
	              PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat(*hDC, &pfd);
	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context */
	*hRC = wglCreateContext(*hDC);
	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent (NULL, NULL);
	wglDeleteContext (hRC);
	ReleaseDC (hWnd, hDC);
}
#endif

void OpenVideo(int width, int height, int fullscreen, int multisample)
{
	int value;

	ScreenWidth = width;
	ScreenHeight = height;
	Fullscreen = fullscreen;

#ifdef USE_SDL
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 	8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 	8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	if (multisample) {
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample );
	}

	if ( SDL_SetVideoMode( ScreenWidth, ScreenHeight, 0, SDL_OPENGL |  (Fullscreen ? SDL_FULLSCREEN : 0) ) == NULL ) {
		fputs("Couldn't set GL mode:\n", stderr);
		fputs(SDL_GetError(), stderr);
		fputs("\n", stderr);
		SDL_Quit();
		exit(1);
	}

	/* le truc a NE PAS oublier sinon psc80 frappe!  */
	if (fullscreen) {
		SDL_ShowCursor( SDL_FALSE );
	}

	printf("Screen BPP: %d\n", SDL_GetVideoSurface()->format->BitsPerPixel);
	printf("\n");
	printf( "Vendor     : %s\n", glGetString( GL_VENDOR ) );
	printf( "Renderer   : %s\n", glGetString( GL_RENDERER ) );
	printf( "Version    : %s\n", glGetString( GL_VERSION ) );
	printf( "Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
	printf("\n");

	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value );
	printf( "SDL_GL_RED_SIZE: %d\n", value);
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value );
	printf( "SDL_GL_GREEN_SIZE: %d\n", value);
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value );
	printf( "SDL_GL_BLUE_SIZE: %d\n", value);
	SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value );
	printf( "SDL_GL_DEPTH_SIZE: %d\n", value );
	SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value );
	printf( "SDL_GL_DOUBLEBUFFER: %d\n", value );

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
	printf( "MAX_TEXTURE_SIZE: %d\n", value );

#else
	WNDCLASS wc;
	MSG msg;
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

	if (hWnd != 0) {
		/* Destroy existing window */
		CloseVideo();

	}

	WindowRect.left = (long)	0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)	0;
	WindowRect.bottom = (long)height;


	/* register window class */
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "knights";
	RegisterClass(&wc);


	if (fullscreen) {
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			fullscreen = false;
		}
	}

	if (fullscreen) {
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(FALSE);
	} else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(hWnd = CreateWindowEx(	dwExStyle, "knights", "E( )3", dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
	                             (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
	                             (GetSystemMetrics(SM_CYSCREEN) - height) / 2,
	                             WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, wc.hInstance, NULL))) {
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(1);
	}


	SetForegroundWindow(hWnd);

	/* enable OpenGL for the window */
	EnableOpenGL(hWnd, &hDC, &hRC);

#endif
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	glClearColor( 1, 0, 0, 1 );
	glEnable(GL_TEXTURE_2D);
}

void CloseVideo()
{
#ifdef USE_SDL
#else
	/* shutdown OpenGL */
	DisableOpenGL(hWnd, hDC, hRC);

	/* destroy the window */
	DestroyWindow(hWnd);
	hWnd = 0;
#endif
}

Uint32 ms_array[MS_SIZE];

const int MS_DELAY = 500;
static int old_ms = 0;
static int old_ms_frames = 0;

static void UpdateTimer()
{
#ifdef USE_SDL
	ticks = SDL_GetTicks();
#else
	ticks = timeGetTime();
#endif
	ms = (ticks - oldTicks);

	if (ticks - old_ms > MS_DELAY) {
		int dframes = frames - old_ms_frames;
		int dticks  = (ticks - old_ms);
		ms = (double) dticks / dframes;
		old_ms = ticks;
		old_ms_frames = frames;
		//printf("%d %d %.3f\n", dframes, dticks, ms);
		int i;
		for (i = 0; i < MS_SIZE - 1; i++) {
			ms_array[i] = ms_array[i+1];
		}
		ms_array[MS_SIZE-1] = ms;
	}

	/* we interpolate delta to get a smooth movement */
	delta = delta * 0.5 + (ms / 20.0) * 0.5;

	frames++;
	timer += delta;

	oldTicks = ticks;
}

void Flip()
{
#ifdef USE_SDL
	SDL_GL_SwapBuffers();
#else
	SwapBuffers(hDC);
#endif
	UpdateTimer();
}

Surface * CreateSurface(int width, int height, int bpp)
{
	Surface *s;
	s = (Surface*) malloc(sizeof(Surface));
	s->width = width;
	s->height = height;
	s->bpp = bpp;
	s->pixels = (Uint8*) malloc(width * height * bpp);
	FillSurface(s, 0);
	//printf("Creating surface (%dx%d)\n",width, height);

	stat_surfaces++;
	stat_surface_mem += width * height * bpp / 1024;

	return s;
}

void FreeSurface(Surface *surface)
{
	//stat_surfaces--;
	//stat_surface_mem-=surface->width*surface->height*surface->bpp;

	free(surface->pixels);
	surface->pixels = NULL;
	free(surface);
	surface = NULL;
}

void FillSurface(Surface *surface, Uint32 color)
{
	switch (surface->bpp) {
	case 1: {
		Uint8* p = (Uint8*)surface->pixels;
		int i;
		for (i = 0; i < surface->width*surface->height; i++)
			*p++ = (Uint8)color;
	}
	break;
	case 2: {
		Uint16* p = (Uint16*)surface->pixels;
		int i;
		for (i = 0; i < surface->width*surface->height; i++)
			*p++ = (Uint16)color;
	}
	break;
	case 3: {
		Uint8* p = surface->pixels;
		int i;
		for (i = 0; i < surface->width*surface->height; i++) {
			*p++ = (Uint8) (color >> 16) & 255;
			*p++ = (Uint8) (color >> 8) & 255;
			*p++ = (Uint8) (color >> 0) & 255;
		}
	}
	break;
	case 4: {
		Uint32* p = (Uint32*)surface->pixels;
		int i;
		for (i = 0; i < surface->width*surface->height; i++)
			*p++ = (Uint32)color;
	}
	break;

	};
}

void BlitSurface(Surface *dest, Surface *source, int x, int y)
{/* not used */
}

static int nibble_byte;
static int nibble_index;

inline int ReadNibble(Uint8** p)
{
	int c;

	if (nibble_index) {
		nibble_index = 0;
		return nibble_byte >> 4;
	} else {
		nibble_index++;
		nibble_byte = *(*p)++;
		return nibble_byte&0x0f;
	}
}

Surface* LoadRLEBitmap(RLEBitmap *rle)
{
	int y;
	Uint8 *p;
	Surface * bmp;
	int color;

	bmp = CreateSurface(NextPowerOfTwo(rle->width), NextPowerOfTwo(rle->height), 4);

	printf("unpacking RLE (%dx%d)\n", rle->width, rle->height);

	nibble_byte = 0;
	nibble_index = 0;

	p = rle->pixels;

	for (y = 0; y < rle->height; y++) {

		int w = rle->width;
		int x = 0;
		int repeat;
		Uint8 c, r, g, b;

		do {
			repeat = 1;

			c = ReadNibble(&p);

			if (c&0x08) {
				repeat = (c & 0x07) + 2;
			} else {
				color = c;
			}
			r = rle->palette[color][0];
			g = rle->palette[color][1];
			b = rle->palette[color][2];
			while (repeat--) {
				if (color) {
					*(((Uint32*)bmp->pixels) + y*bmp->width + x) =
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
					    (255 << 24) + (r << 16) + (g << 8) + (b << 0);
#else
					    (255 << 0) + (r << 8) + (g << 16) + (b << 24);
#endif
				}
				x++;
			}
			if (x > w) {
				printf("fatal error: y=%d x=%d\n", y, x);
				fflush(stdout);
				//exit(1);
			}
		} while (x < w);
	}


	return bmp;
}




GLuint CreateTexture(int width, int height)
{
	GLuint texture;
	Surface * surface;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);

	printf("Creating texture (%dx%d)\n", width, height);

	surface = CreateSurface(width, height, 4);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	UploadTexture(texture, surface);
	FreeSurface(surface);

	stat_textures++;
	stat_texture_mem += width * height * 4 / 1024;

	return texture;
}

void FreeTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

void UploadTexture(GLuint texture, Surface *surface)
{
	printf("Uploading texture (%dx%d)\n", surface->width, surface->height);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->width, surface->height,
	             0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
}

void UploadTextureMipmap(GLuint texture, Surface *surface)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->width, surface->height,
	                  GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
}

void UploadTextureSub(GLuint texture, Surface *surface, int x, int y)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, surface->width, surface->height,
	                 GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
}

void UploadTextureLuminance(GLuint texture, Surface *surface)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, surface->width, surface->height,
	             0, GL_LUMINANCE, GL_UNSIGNED_BYTE, surface->pixels);
}

void UploadTextureAlpha(GLuint texture, Surface *surface)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, surface->width, surface->height,
	             0, GL_ALPHA, GL_UNSIGNED_BYTE, surface->pixels);
}

int NextPowerOfTwo(int number)
{
	int value = 1;

	while ( value < number ) {
		value <<= 1;
	}
	return value;
}

void Mode2D()
{
	glViewport(0, 0, ScreenWidth, ScreenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, 1024.0, 768.0, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float SineEx(double timer, float min, float max, float period, float offset)
{
	float base, intensity;
	intensity = (max - min) / 2;
	base = (max + min) / 2;
	return  base + intensity*sin((timer*20.0 + offset)*TWOPI / period);
}

float Sine(float min, float max, float period, float offset)
{
	return SineEx(timer, min, max, period, offset);
}

float frand()
{
	int i;
	i = rand();
	if (i == 0) {
		return 0;
	} else {
		return (i % 1000) / 1000.0f;
	}
}

Surface * Scale(Surface * src, float scale)
{
	Surface * dst;
	int x, y, i;
	Uint8 *s, *d;
	dst = CreateSurface(src->width * scale, src->height * scale, 4);

	gluScaleImage(GL_RGBA, src->width, src->height, GL_UNSIGNED_BYTE, src->pixels, dst->width, dst->height, GL_UNSIGNED_BYTE, dst->pixels);
	return dst;
}

void Blur(Surface * src, int repeat)
{
	int x, y, n, i, j, pass;
	Uint8 *s;

	for (pass = 0; pass < repeat; pass++) {
		for (y = 1; y < src->height - 1; y++) {
			for (x = 1; x < src->width - 1; x++) {
				s = src->pixels + y * src->width * 4 + x * 4;
				for (n = 0; n < 4; n++) {
					int sum = 0;
					for (j = -1; j <= 1; j++) {
						for (i = -1; i <= 1; i++) {
							sum += (*(s + j * src->width * 4 + 4 * i));
						}
					}
					sum /= 9;
					*s = sum;
					s++;
				}
			}
		}
	}
}

void Noisify(Surface * s, int noise)
{
	int i, j, n;
	Uint8 *p;

	p = s->pixels;
	i = s->width * s->height;

	while (i--) {
		j = s->bpp;
		n = ( (rand() + rand() / 1337) % (noise * 2)) - noise;
		while (j--) {
			int c = *p;
			c += n;
			if ( c < 0 ) c = 0;
			if ( c > 255 ) c = 255;
			*p = c;
			p++;
		}
	}


}

void AddBorder(Surface * surface, Uint32 color)
{
	int x, y, i;
	int offset[4] = { -surface->width - 1, -surface->width + 1, surface->width - 1, surface->width + 1 };

	for (y = 1; y < surface->height - 1; y++) {
		Uint32 *p = (Uint32*) (surface->pixels + y * surface->width * 4);
		for (x = 1; x < surface->width - 1; x++) {
			if (*p == 0xffffffff) {
				for (i = 0; i < 4; i++)
					if (*(p + offset[i]) != 0xffffffff) {
						*(p + offset[i]) = color;
					}
			}
			p++;
		}
	}
}


void GenCube()
{
	//glTexCoord2f(0,0);
	glVertex3f( 0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f( 0.5f, 0.5f, 0.5f);

	//glTexCoord2f(1,0);
	glVertex3f( 0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f( 0.5f, -0.5f, -0.5f);

	//glTexCoord2f(0,1);
	glVertex3f( 0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f( 0.5f, -0.5f, 0.5f);

	//glTexCoord2f(1,0);
	glVertex3f( 0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f( 0.5f, 0.5f, -0.5f);

	//glTexCoord2f(0,1);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	//glTexCoord2f(1,1);
	glVertex3f( 0.5f, 0.5f, -0.5f);
	glVertex3f( 0.5f, 0.5f, 0.5f);
	glVertex3f( 0.5f, -0.5f, 0.5f);
	glVertex3f( 0.5f, -0.5f, -0.5f);
}

static char fontWidths[256];

int GetStringWidth(char* str)
{
	char *p;
	float width = 0;

	p = str;

	while (*p) {
		width += fontWidths[(*p)-32] - 1;
		p++;
	}
	return width;
}

void DrawString(float x, float y, char* str, TextPlacement place, GLint texFont, float size)
{
	char *p;
	float width = 0;
	int i;

	width = GetStringWidth(str) * size;

	if (place == TEXT_CENTER) {
		x -= width / 2;
		y -= size * 16 / 2;
	} else if (place == TEXT_RIGHT) {
		x -= width;
	}

	p = str;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glBindTexture(GL_TEXTURE_2D, texFont);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	while (*p) {
		float u, v;
		float su, sv;
		char c = (*p) - 32;
		float w, h;


		w = size * fontWidths[c];
		h = size * 16;

		if (c > 0) {


			su = fontWidths[c] / 256.0;
			sv = 1.0 / 8.0;

			u = (float)(c % 16) / 16.0;
			v = (float)(c / 16) / 8.0;

			glBegin(GL_TRIANGLE_STRIP);
			glColor4f(1, 1, 1, 1);
			glTexCoord2f(u, v);
			glVertex3f(x, y, 0.0f);
			glTexCoord2f(u + su, v);
			glVertex3f(x + w, y, 0.0f);
			glTexCoord2f(u, v + sv);
			glVertex3f(x, y + h, 0.0f);
			glTexCoord2f(u + su, v + sv);
			glVertex3f(x + w, y + h, 0.0f);
			glEnd();
		}
		x += w - size;
		p++;
	}

	glDisable(GL_ALPHA_TEST);
}

void InitFont(Surface * bmpFont)
{
	int i, j, x, y;
	for (j = 0; j < 6; j++) {
		for (i = 0; i < 16; i++) {
			Uint32* bmp = ((Uint32*)(bmpFont->pixels)) + bmpFont->width * j * 16 + i * 16;
			int width = 0;
			for (x = 15; x >= 0; x--) {
				int empty = 1;
				for (y = 0; y < 16; y++) {
					if (*(bmp + y*bmpFont->width + x)) {
						empty = 0;
						break;
					}
				}
				if (!empty) {
					width = x + 1;
					break;
				}
			}
			fontWidths[j*16+i] = width;
		}
	}
	fontWidths[0] = 4;
}

/*
void AddShadow(Surface * surface, int sx, int sy)
{
	int x,y;
	int offset = sy*surface->width+sx;
	Surface *tmp;
	tmp = CreateSurface(surface->width, surface->height, 4);

	for(y=0; y<surface->height; y++) {
		Uint32 *p = (Uint32*) (surface->pixels+y*surface->width*4);
		Uint32 *s = (Uint32*) (tmp->pixels+y*tmp->width*4);
		for(x=0; x<surface->width; x++) {
			Uint8 alpha;
			alpha = ((*p)&0xff000000)>>24 ;
			if (alpha>128) {
				if ( (y+sy < surface->height) &&
					(x+sx < surface->width) ) {
					if ( ((*(p+offset))&0xff000000) == 0) {
						*(p+offset) = 127<<24;
					}
				}
			}
			p++;
			s++;
		}
	}
}
*/

void Threshold(Surface * s, int threshold, int smoothing, int min, int max)
{
	int x, y, i;
	Uint8 table[256];

	for (i = 0; i < 256; i++) {
		if (i < threshold - smoothing) {
			table[i] = min;
		} else if (i >= threshold + smoothing) {
			table[i] = max;
		} else {
			table[i] = min + (max - min) / 256.0 * fade((i - (threshold - smoothing)) * (127.0 / smoothing) / 256.0) * 256.0;
		}
	}

	for (y = 0; y < s->width; y++) {
		Uint8 *dst = (Uint8*)(s->pixels) + y * s->width;
		for (x = 0; x < s->height; x++) {
			*dst++ = table[*dst];
		}
	}
}

/* hue 0-360 - s,v,a 0->1*/
void ColorHSV( float h, float s, float v, float *r, float *g, float *b)
{
	int i;
	float f, p, q, t;

	if ( s == 0 ) {
		*r = *g = *b = v;
		return;
	}

	h = fmod(h, 360);

	h /= 60;
	i = floor( h );
	f = h - i;
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch ( i ) {
	case 0:
		*r = v;
		*g = t;
		*b = p;
		break;
	case 1:
		*r = q;
		*g = v;
		*b = p;
		break;
	case 2:
		*r = p;
		*g = v;
		*b = t;
		break;
	case 3:
		*r = p;
		*g = q;
		*b = v;
		break;
	case 4:
		*r = t;
		*g = p;
		*b = v;
		break;
	default:
		*r = v;
		*g = p;
		*b = q;
		break;
	}
}

void SaveBMP(Surface * bmp, char * filename)
{
#ifdef USE_SDL
	SDL_Surface* image;

	if (bmp->bpp == 1) {
		image = SDL_CreateRGBSurface(SDL_SWSURFACE, bmp->width, bmp->height, 8, 0xf000, 0xf00, 0x0f0, 0x00f);
	} else {
		image = SDL_CreateRGBSurface(SDL_SWSURFACE, bmp->width, bmp->height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	}
	memcpy(image->pixels, bmp->pixels, bmp->width*bmp->height*bmp->bpp);
	SDL_SaveBMP(image, filename);
#endif
}

Surface * LoadBMP(char * filename)
{
#ifdef USE_SDL
	Surface * image;
	SDL_Surface * bmp;
	SDL_Surface * bmp2;

	bmp = SDL_LoadBMP(filename);
	if (!bmp) {
		return NULL;
	}

	if (bmp->format->BytesPerPixel == 1) {
		bmp2 = SDL_CreateRGBSurface(SDL_SWSURFACE, bmp->w, bmp->h, 8, 0xf000, 0xf00, 0x0f0, 0x00f);
	} else {
		bmp2 = SDL_CreateRGBSurface(SDL_SWSURFACE, bmp->w, bmp->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	}

	SDL_BlitSurface(bmp, NULL, bmp2, NULL);

	image = CreateSurface(bmp2->w, bmp2->h, bmp2->format->BytesPerPixel);
	memcpy(image->pixels, bmp2->pixels, bmp2->w*bmp2->h*bmp2->format->BytesPerPixel);

	free(bmp);
	free(bmp2);

	return image;
#else
	return NULL;
#endif
}

/***************************************************************************/
#ifdef USE_MIKMOD

#define USE_RWOPS
#include "mikmod/mikmod.h"

MODULE *module;
SDL_RWops *op;
void * module_data;
int module_size;

#define FREQ 44100
#define BUFFERSIZE 2048
#define SAMPLESIZE 16
#define INTERPOLATION 1

int lockaudio = 0;

void SDLMikMod_FillAudio(void *udata, Uint8 *stream, int len)
{
	lockaudio = 1;
	if (Player_Active()) {
		VC_WriteBytes((Sint8 *)stream, len);
	} else {
		VC_SilenceBytes((Sint8 *)stream, len);
	}
	lockaudio = 0;
}

void OpenMusic(void * pModule, int size)
{
	SDL_AudioSpec wanted;
	SDL_AudioSpec fmt;

	wanted.freq = FREQ;

	/* 16bits or 8 bits ?*/
	if ( SAMPLESIZE == 16) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		wanted.format = AUDIO_S16LSB;
#else
		wanted.format = AUDIO_S16MSB;
#endif
		md_mode = DMODE_16BITS;
	} else {
		wanted.format = AUDIO_U8;
		md_mode = 0;
	}
	wanted.channels = 2;

	wanted.samples = BUFFERSIZE;
	wanted.callback = SDLMikMod_FillAudio;
	wanted.userdata = NULL;

	SDL_OpenAudio(&wanted, &fmt);

	if ((fmt.format&0xFF) == 8) {
		md_mode -= DMODE_16BITS;
	}

	if (fmt.channels == 2) {
		md_mode += DMODE_STEREO;
	}

	if (INTERPOLATION) {
		md_mode += DMODE_INTERP;
	}

	md_mixfreq = FREQ;
	md_device   = 0;
	md_volume   = 128;
	md_musicvolume = 128;
	md_sndfxvolume = 128;
	md_pansep   = 128;
	md_reverb   = 0;

	MikMod_RegisterAllLoaders();
	MikMod_RegisterAllDrivers();

	MikMod_Init(NULL);

	SDL_PauseAudio(0);

	module_data = pModule;
	module_size = size;

	op = SDL_RWFromMem(module_data, module_size);
	module = Player_LoadRW(op, 32, 0);
	Player_Start(module);

	//Player_SetVolume(0);
	Player_SetSynchroValue(-1);

	if (!module) {
		fputs("error loading module!", stderr);
		exit(1);
		return;
	}
}

void CloseMusic()
{
	Player_Stop();
	SDL_PauseAudio(1);
	SDL_CloseAudio();
	MikMod_Exit();
}

int GetMusicPosition()
{
	int pattern, row;
	pattern = module->sngpos;
	row = module->patpos;
	return 256*pattern + row;
}

void SetMusicPosition(int pattern)
{
	Player_SetPosition(pattern);
}

/*int GetSynchro()
{
	int s;

	if (lockaudio>0) return;

	s = Player_GetSynchroValue();
	Player_SetSynchroValue(-1);
	return s;
}*/


#else
#define __COMPILE_MINIFMOD__
#include "minifmod/minifmod.h"

FMUSIC_MODULE *module;
void * module_data;
int module_size;

int GetMusicPosition()
{
	int pattern, row;
	pattern = FMUSIC_GetOrder(module);
	row = FMUSIC_GetRow(module);
	return 256*pattern + row;
}

void SetMusicPosition(int pattern)
{
	module->nextorder = pattern;
	module->nextrow = 0;
}

typedef struct {
	int length;
	int pos;
	void *data;
} MEMFILE;

MEMFILE fmod_MemFile;

static unsigned int fmod_memopen(char *name)
{
	char str[80];

	fmod_MemFile.length = module_size;
	fmod_MemFile.data = module_data;
	fmod_MemFile.pos = 0;

	return 1;
}

static void fmod_memclose(unsigned int handle)
{
}

static int fmod_memread(void *buffer, int size, unsigned int handle)
{
	if (fmod_MemFile.pos + size >= fmod_MemFile.length)
		size = fmod_MemFile.length - fmod_MemFile.pos;

	memcpy(buffer, (char *)fmod_MemFile.data + fmod_MemFile.pos, size);
	fmod_MemFile.pos += size;

	return(size);
}

static void fmod_memseek(unsigned int handle, int pos, signed char mode)
{
	if (mode == SEEK_SET)
		fmod_MemFile.pos = pos;
	else if (mode == SEEK_CUR)
		fmod_MemFile.pos += pos;
	else if (mode == SEEK_END)
		fmod_MemFile.pos = fmod_MemFile.length + pos;

	if (fmod_MemFile.pos > fmod_MemFile.length)
		fmod_MemFile.pos = fmod_MemFile.length;
}

static int fmod_memtell(unsigned int handle)
{
	return(fmod_MemFile.pos);
}

void OpenMusic(void * pModule, int size)
{
	module_data = pModule;
	module_size = size;

	FSOUND_File_SetCallbacks(fmod_memopen, fmod_memclose, fmod_memread, fmod_memseek, fmod_memtell);
	if (!FSOUND_Init(44100, 0)) {
		fputs("error opening sound device !", stderr);
	}
	module = FMUSIC_LoadSong(NULL, NULL);
	if (!module) {
		fputs("error loading module!", stderr);
		FSOUND_Close();
		return;
	}
	if (!FMUSIC_PlaySong(module)) {
		fputs("error starting module!", stderr);
		FSOUND_Close();
	}
}

void CloseMusic()
{
	FMUSIC_StopSong(module);
	FMUSIC_FreeSong(module);
	FSOUND_Close();
}

/*int GetSynchro()
{
	return FMUSIC_GetSynchro(module);
}*/

#endif

static int lastSynchro = 0;
bool GetSynchro(int* list)
{
	int pos = GetMusicPosition();
	while (*list) {
		if (*list == pos) {
			if (lastSynchro != pos) {
				lastSynchro = pos;
				return true;
			}
		}
		list++;
	}
	return false;
}


