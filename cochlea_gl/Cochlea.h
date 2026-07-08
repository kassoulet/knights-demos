#ifndef COCHLEA_GL_H
#define COCHLEA_GL_H

#include "OpenGLWrapper.h"

// Define Windows types that are used in the original code
#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef HRESULT
typedef long HRESULT;
#define S_OK 0L
#define S_FALSE 1L
#define E_FAIL -1L
#define E_OUTOFMEMORY -2L
#endif

#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

#ifndef FLOAT
typedef float FLOAT;
#endif

// Define Direct3D compatibility constants for easy conversion
#define D3DPT_TRIANGLELIST 4
#define D3DPT_TRIANGLESTRIP 5
#define D3DPT_LINELIST 1

// Render state constants
#define D3DRENDERSTATE_ALPHABLENDENABLE 0
#define D3DRENDERSTATE_SRCBLEND 1
#define D3DRENDERSTATE_DESTBLEND 2

#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6
#define D3DBLEND_ONE 2
#define D3DBLEND_ZERO 1
#define D3DBLEND_SRCCOLOR 3

// Global variables that were in the original code
extern float VBLframe;
extern float VBLdelta;
extern int VBLsync;

// Set by PartScroll() when the demo sequence finishes, instead of calling
// exit() directly - that used to terminate the process immediately from deep
// inside the render loop, skipping main()'s controlled shutdown (closing the
// audio device, freeing the loaded module, tearing down the GL context).
extern bool g_demoFinished;

// Function declarations
void AppPause(BOOL);
HRESULT App_InitDeviceObjects();
void App_DeleteDeviceObjects();
HRESULT RenderParticles();
int MIDASgetPosition(void);
void MIDASerror(void);
void Error(char *string, ...);

// Rendering functions
void Rotozoom(char * map, float angle, float zoom, float xoffset, float yoffset, float alpha);
void DrawZoomSprite(char * sprite, float x1, float y1, float x2, float y2, float a, float z);
void DrawAddSprite(char * sprite, float x1, float y1, float x2, float y2, float a1, float a2);
void DrawBackground(char * sprite, float zoom);
void DrawRect(float x1, float y1, float x2, float y2, float c);
void DrawAddRect(float x1, float y1, float x2, float y2, float c);
void DrawSubRect(float x1, float y1, float x2, float y2, float c);

// Part functions
void PartBegin();
void PartMyGirl();
void PartScroll();
void PartFire();
void PartPhantasm();
void PartPurple();

// Main rendering function
HRESULT App_Render();

#endif // COCHLEA_GL_H