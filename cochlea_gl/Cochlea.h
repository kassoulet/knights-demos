#ifndef COCHLEA_GL_H
#define COCHLEA_GL_H

#include "OpenGLWrapper.h"
#include <vector>
#include <cstring>

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

// Color macros
#define D3DRGB(r, g, b) (float)((unsigned char)((r) * 255.0f)), (float)((unsigned char)((g) * 255.0f)), (float)((unsigned char)((b) * 255.0f)), 1.0f
#define D3DRGBA(r, g, b, a) (float)((unsigned char)((r) * 255.0f)), (float)((unsigned char)((g) * 255.0f)), (float)((unsigned char)((b) * 255.0f)), (float)((unsigned char)((a) * 255.0f))

// Render state constants
#define D3DRENDERSTATE_ALPHABLENDENABLE 0
#define D3DRENDERSTATE_SRCBLEND 1
#define D3DRENDERSTATE_DESTBLEND 2

#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6
#define D3DBLEND_ONE 2
#define D3DBLEND_ZERO 1
#define D3DBLEND_SRCCOLOR 3

// Texture stage states
#define D3DTSS_COLORARG1 0
#define D3DTSS_COLORARG2 1
#define D3DTSS_COLOROP 2
#define D3DTSS_ALPHAARG1 3
#define D3DTSS_ALPHAARG2 4
#define D3DTSS_ALPHAOP 5
#define D3DTSS_MINFILTER 6
#define D3DTSS_MAGFILTER 7

#define D3DTOP_MODULATE 4

// Virtual key codes
#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_LEFT 0x25
#define VK_RIGHT 0x27

// Function to simulate GetAsyncKeyState (placeholder - will always return 0)
inline short GetAsyncKeyState(int key) {
    // In a real implementation, this would check for key states
    // For now, return 0 to indicate no key is pressed
    return 0;
}

// Function to simulate GetTempPath (placeholder)
inline DWORD GetTempPath(DWORD nBufferLength, char* lpBuffer) {
    // In a real implementation, this would get the temp path
    // For now, return a simple temp directory
    if (nBufferLength > 10) {
        strcpy(lpBuffer, "/tmp/");
        return 5; // length of "/tmp/"
    }
    return 0;
}

// Structure to represent a vertex (similar to D3DTLVERTEX)
struct D3DTLVERTEX {
    float x, y, z;      // Position
    float rhw;          // Reciprocal homogeneous W
    float color;        // Color (packed ARGB)
    float specular;     // Specular color (not used in 2D)
    float tu, tv;       // Texture coordinates
    
    // Constructor
    D3DTLVERTEX() : x(0), y(0), z(0), rhw(0), color(0), specular(0), tu(0), tv(0) {}
    D3DTLVERTEX(float _x, float _y, float _z, float _rhw, float _color, float _spec, float _tu, float _tv)
        : x(_x), y(_y), z(_z), rhw(_rhw), color(_color), specular(_spec), tu(_tu), tv(_tv) {}
};

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