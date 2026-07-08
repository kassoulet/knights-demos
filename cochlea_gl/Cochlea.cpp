/*-----------------------------------------------------------------------------
	Cochlea OpenGL Version
	(c) KNIGHTS 1999, adapted for OpenGL 2025

	Coded by Gautier 'Impulse' PORTET [impulse@citeweb.net]
	Adapted for OpenGL by Qwen Code

	december 1999 (based on RubK), adapted for OpenGL in 2025
  -----------------------------------------------------------------------------*/

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "Cochlea.h"
#include "OpenGLWrapper.h"

#include "cochlea.xm.c"

#define MUSIC 1

#ifdef MUSIC
#include "MIDASDLL.H"
#include "xm_player.h"
MIDASmodule module;
MIDASmodulePlayHandle playHandle;

// Global XM player instance
xm_player_t* g_xmPlayer = nullptr;
extern int g_sample_rate;

// Stub implementations for MIDAS functions that use the XM player
int MIDASgetLastError(void) { return 0; }
char* MIDASgetErrorMessage(int errorCode) { return (char*)"No error"; }
DWORD MIDASgetDisplayRefreshRate(void) { return 60; }
BOOL MIDASstartup(void) { return TRUE; }
BOOL MIDASdetectSD(void) { return TRUE; }
BOOL MIDASdetectSoundCard(void) { return TRUE; }
BOOL MIDASconfig(void) { return TRUE; }
BOOL MIDASloadConfig(char *fileName) { return TRUE; }
BOOL MIDASsaveConfig(char *fileName) { return TRUE; }
BOOL MIDASreadConfigRegistry(DWORD key, char *subKey) { return TRUE; }
BOOL MIDASwriteConfigRegistry(DWORD key, char *subKey) { return TRUE; }
BOOL MIDASinit(void) { return TRUE; }
BOOL MIDASsetOption(int option, DWORD value) { return TRUE; }
DWORD MIDASgetOption(int option) { return 0; }
BOOL MIDASclose(void) { return TRUE; }
BOOL MIDASsuspend(void) { return TRUE; }
BOOL MIDASresume(void) { return TRUE; }
BOOL MIDASopenChannels(int numChannels) { return TRUE; }
BOOL MIDAScloseChannels(void) { return TRUE; }
BOOL MIDASsetAmplification(DWORD amplification) { return TRUE; }
BOOL MIDASstartBackgroundPlay(DWORD pollRate) { return TRUE; }
BOOL MIDASstopBackgroundPlay(void) { return TRUE; }
BOOL MIDASpoll(void) { return TRUE; }
void MIDASlock(void) { }
void MIDASunlock(void) { }
char* MIDASgetVersionString(void) { return (char*)"1.0"; }
BOOL MIDASsetTimerCallbacks(DWORD rate, BOOL displaySync, void (MIDAS_CALL *preVR)(), void (MIDAS_CALL *immVR)(), void (MIDAS_CALL *inVR)()) { return TRUE; }
BOOL MIDASremoveTimerCallbacks(void) { return TRUE; }
DWORD MIDASallocateChannel(void) { return 0; }
BOOL MIDASfreeChannel(DWORD channel) { return TRUE; }
MIDASmodule MIDASloadModule(char *fileName) { return (MIDASmodule)1; }
MIDASmodulePlayHandle MIDASplayModule(MIDASmodule module, BOOL loopSong) { return (MIDASmodulePlayHandle)1; }
MIDASmodulePlayHandle MIDASplayModuleSection(MIDASmodule module, unsigned startPos, unsigned endPos, unsigned restartPos, BOOL loopSong) { return (MIDASmodulePlayHandle)1; }
BOOL MIDASstopModule(MIDASmodulePlayHandle playHandle) { return TRUE; }
BOOL MIDASgetPlayStatus(MIDASmodulePlayHandle playHandle, MIDASplayStatus *status) {
    if (status) {
        status->position = 0;
        status->pattern = 0;
        status->row = 0;
        status->syncInfo = 0;
        status->songLoopCount = 0;
    }
    return TRUE;
}
BOOL MIDASsetPosition(MIDASmodulePlayHandle playHandle, int newPosition) { return TRUE; }
BOOL MIDASsetMusicVolume(MIDASmodulePlayHandle playHandle, unsigned volume) { return TRUE; }
BOOL MIDASgetModuleInfo(MIDASmodule module, MIDASmoduleInfo *info) { return TRUE; }
BOOL MIDASgetInstrumentInfo(MIDASmodule module, int instNum, MIDASinstrumentInfo *info) { return TRUE; }
BOOL MIDASsetMusicSyncCallback(MIDASmodulePlayHandle playHandle, void (MIDAS_CALL *callback)(unsigned syncInfo, unsigned position, unsigned row)) { return TRUE; }
BOOL MIDASfadeMusicChannel(MIDASmodulePlayHandle playHandle, unsigned channel, unsigned fade) { return TRUE; }
MIDASsample MIDASloadRawSample(char *fileName, int sampleType, int loopSample) { return (MIDASsample)1; }
MIDASsample MIDASloadWaveSample(char *fileName, int loopSample) { return (MIDASsample)1; }
BOOL MIDASfreeSample(MIDASsample sample) { return TRUE; }
BOOL MIDASallocAutoEffectChannels(unsigned numChannels) { return TRUE; }
BOOL MIDASfreeAutoEffectChannels(void) { return TRUE; }
MIDASsamplePlayHandle MIDASplaySample(MIDASsample sample, unsigned channel, int priority, unsigned rate, unsigned volume, int panning) { return (MIDASsamplePlayHandle)1; }
BOOL MIDASstopSample(MIDASsamplePlayHandle sample) { return TRUE; }
BOOL MIDASsetSampleRate(MIDASsamplePlayHandle sample, unsigned rate) { return TRUE; }
BOOL MIDASsetSampleVolume(MIDASsamplePlayHandle sample, unsigned volume) { return TRUE; }
BOOL MIDASsetSamplePanning(MIDASsamplePlayHandle sample, int panning) { return TRUE; }
BOOL MIDASsetSamplePriority(MIDASsamplePlayHandle sample, int priority) { return TRUE; }
DWORD MIDASgetSamplePlayStatus(MIDASsamplePlayHandle sample) { return 0; }
MIDASstreamHandle MIDASplayStreamFile(char *fileName, unsigned sampleType, unsigned sampleRate, unsigned bufferLength, int loopStream) { return (MIDASstreamHandle)1; }
BOOL MIDASstopStream(MIDASstreamHandle stream) { return TRUE; }
MIDASstreamHandle MIDASplayStreamWaveFile(char *fileName, unsigned bufferLength, int loopStream) { return (MIDASstreamHandle)1; }
MIDASstreamHandle MIDASplayStreamPolling(unsigned sampleType, unsigned sampleRate, unsigned bufferLength) { return (MIDASstreamHandle)1; }
unsigned MIDASfeedStreamData(MIDASstreamHandle stream, unsigned char *data, unsigned numBytes, BOOL feedAll) { return 0; }
BOOL MIDASsetStreamRate(MIDASstreamHandle stream, unsigned rate) { return TRUE; }
BOOL MIDASsetStreamVolume(MIDASstreamHandle stream, unsigned volume) { return TRUE; }
BOOL MIDASsetStreamPanning(MIDASstreamHandle stream, int panning) { return TRUE; }
DWORD MIDASgetStreamBytesBuffered(MIDASstreamHandle stream) { return 0; }
BOOL MIDASpauseStream(MIDASstreamHandle stream) { return TRUE; }
BOOL MIDASresumeStream(MIDASstreamHandle stream) { return TRUE; }
MIDASechoHandle MIDASaddEchoEffect(MIDASechoSet *echoSet) { return (MIDASechoHandle)1; }
BOOL MIDASremoveEchoEffect(MIDASechoHandle echoHandle) { return TRUE; }
BOOL MIDASaddPostProcessor(MIDASpostProcessor *postProc, unsigned procPos, void *userData) { return TRUE; }
BOOL MIDASremovePostProcessor(MIDASpostProcessor *postProc) { return TRUE; }
BOOL MIDASfreeModule(MIDASmodule module) { return TRUE; }

#else
// Define stubs when MUSIC is not defined
int MIDASgetLastError(void) { return 0; }
char* MIDASgetErrorMessage(int errorCode) { return (char*)"No error"; }
DWORD MIDASgetDisplayRefreshRate(void) { return 60; }
BOOL MIDASstartup(void) { return TRUE; }
BOOL MIDASdetectSD(void) { return TRUE; }
BOOL MIDASdetectSoundCard(void) { return TRUE; }
BOOL MIDASconfig(void) { return TRUE; }
BOOL MIDASloadConfig(char *fileName) { return TRUE; }
BOOL MIDASsaveConfig(char *fileName) { return TRUE; }
BOOL MIDASreadConfigRegistry(DWORD key, char *subKey) { return TRUE; }
BOOL MIDASwriteConfigRegistry(DWORD key, char *subKey) { return TRUE; }
BOOL MIDASinit(void) { return TRUE; }
BOOL MIDASsetOption(int option, DWORD value) { return TRUE; }
DWORD MIDASgetOption(int option) { return 0; }
BOOL MIDASclose(void) { return TRUE; }
BOOL MIDASsuspend(void) { return TRUE; }
BOOL MIDASresume(void) { return TRUE; }
BOOL MIDASopenChannels(int numChannels) { return TRUE; }
BOOL MIDAScloseChannels(void) { return TRUE; }
BOOL MIDASsetAmplification(DWORD amplification) { return TRUE; }
BOOL MIDASstartBackgroundPlay(DWORD pollRate) { return TRUE; }
BOOL MIDASstopBackgroundPlay(void) { return TRUE; }
BOOL MIDASpoll(void) { return TRUE; }
void MIDASlock(void) { }
void MIDASunlock(void) { }
char* MIDASgetVersionString(void) { return (char*)"1.0"; }
BOOL MIDASsetTimerCallbacks(DWORD rate, BOOL displaySync, void (MIDAS_CALL *preVR)(), void (MIDAS_CALL *immVR)(), void (MIDAS_CALL *inVR)()) { return TRUE; }
BOOL MIDASremoveTimerCallbacks(void) { return TRUE; }
DWORD MIDASallocateChannel(void) { return 0; }
BOOL MIDASfreeChannel(DWORD channel) { return TRUE; }
MIDASmodule MIDASloadModule(char *fileName) { return (MIDASmodule)1; }
MIDASmodulePlayHandle MIDASplayModule(MIDASmodule module, BOOL loopSong) { return (MIDASmodulePlayHandle)1; }
MIDASmodulePlayHandle MIDASplayModuleSection(MIDASmodule module, unsigned startPos, unsigned endPos, unsigned restartPos, BOOL loopSong) { return (MIDASmodulePlayHandle)1; }
BOOL MIDASstopModule(MIDASmodulePlayHandle playHandle) { return TRUE; }
BOOL MIDASgetPlayStatus(MIDASmodulePlayHandle playHandle, MIDASplayStatus *status) {
    if (status) {
        status->position = 0;
        status->pattern = 0;
        status->row = 0;
        status->syncInfo = 0;
        status->songLoopCount = 0;
    }
    return TRUE;
}
BOOL MIDASsetPosition(MIDASmodulePlayHandle playHandle, int newPosition) { return TRUE; }
BOOL MIDASsetMusicVolume(MIDASmodulePlayHandle playHandle, unsigned volume) { return TRUE; }
BOOL MIDASgetModuleInfo(MIDASmodule module, MIDASmoduleInfo *info) { return TRUE; }
BOOL MIDASgetInstrumentInfo(MIDASmodule module, int instNum, MIDASinstrumentInfo *info) { return TRUE; }
BOOL MIDASsetMusicSyncCallback(MIDASmodulePlayHandle playHandle, void (MIDAS_CALL *callback)(unsigned syncInfo, unsigned position, unsigned row)) { return TRUE; }
BOOL MIDASfadeMusicChannel(MIDASmodulePlayHandle playHandle, unsigned channel, unsigned fade) { return TRUE; }
MIDASsample MIDASloadRawSample(char *fileName, int sampleType, int loopSample) { return (MIDASsample)1; }
MIDASsample MIDASloadWaveSample(char *fileName, int loopSample) { return (MIDASsample)1; }
BOOL MIDASfreeSample(MIDASsample sample) { return TRUE; }
BOOL MIDASallocAutoEffectChannels(unsigned numChannels) { return TRUE; }
BOOL MIDASfreeAutoEffectChannels(void) { return TRUE; }
MIDASsamplePlayHandle MIDASplaySample(MIDASsample sample, unsigned channel, int priority, unsigned rate, unsigned volume, int panning) { return (MIDASsamplePlayHandle)1; }
BOOL MIDASstopSample(MIDASsamplePlayHandle sample) { return TRUE; }
BOOL MIDASsetSampleRate(MIDASsamplePlayHandle sample, unsigned rate) { return TRUE; }
BOOL MIDASsetSampleVolume(MIDASsamplePlayHandle sample, unsigned volume) { return TRUE; }
BOOL MIDASsetSamplePanning(MIDASsamplePlayHandle sample, int panning) { return TRUE; }
BOOL MIDASsetSamplePriority(MIDASsamplePlayHandle sample, int priority) { return TRUE; }
DWORD MIDASgetSamplePlayStatus(MIDASsamplePlayHandle sample) { return 0; }
MIDASstreamHandle MIDASplayStreamFile(char *fileName, unsigned sampleType, unsigned sampleRate, unsigned bufferLength, int loopStream) { return (MIDASstreamHandle)1; }
BOOL MIDASstopStream(MIDASstreamHandle stream) { return TRUE; }
MIDASstreamHandle MIDASplayStreamWaveFile(char *fileName, unsigned bufferLength, int loopStream) { return (MIDASstreamHandle)1; }
MIDASstreamHandle MIDASplayStreamPolling(unsigned sampleType, unsigned sampleRate, unsigned bufferLength) { return (MIDASstreamHandle)1; }
unsigned MIDASfeedStreamData(MIDASstreamHandle stream, unsigned char *data, unsigned numBytes, BOOL feedAll) { return 0; }
BOOL MIDASsetStreamRate(MIDASstreamHandle stream, unsigned rate) { return TRUE; }
BOOL MIDASsetStreamVolume(MIDASstreamHandle stream, unsigned volume) { return TRUE; }
BOOL MIDASsetStreamPanning(MIDASstreamHandle stream, int panning) { return TRUE; }
DWORD MIDASgetStreamBytesBuffered(MIDASstreamHandle stream) { return 0; }
BOOL MIDASpauseStream(MIDASstreamHandle stream) { return TRUE; }
BOOL MIDASresumeStream(MIDASstreamHandle stream) { return TRUE; }
MIDASechoHandle MIDASaddEchoEffect(MIDASechoSet *echoSet) { return (MIDASechoHandle)1; }
BOOL MIDASremoveEchoEffect(MIDASechoHandle echoHandle) { return TRUE; }
BOOL MIDASaddPostProcessor(MIDASpostProcessor *postProc, unsigned procPos, void *userData) { return TRUE; }
BOOL MIDASremovePostProcessor(MIDASpostProcessor *postProc) { return TRUE; }
#endif

typedef unsigned short word;

//-----------------------------------------------------------------------------
// Declare the application globals for use
//-----------------------------------------------------------------------------
char* g_strAppTitle       = "Cochlea OpenGL";
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;

#define RAD(x) (((double)(x))*0.02454369261)

//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
void    AppPause( BOOL );
HRESULT App_InitDeviceObjects();
void    App_DeleteDeviceObjects();
HRESULT RenderParticles();

#define rnd() (((FLOAT)rand()-(FLOAT)rand())/(2L*RAND_MAX))
#define RND() (((FLOAT)rand())/RAND_MAX)

//-----------------------------------------------------------------------------
// Name: Particle
// Desc: Data structure for each particle
//-----------------------------------------------------------------------------

#define NUM_FIREPARTICLES 300
#define NUM_VIOLETPARTICLES 200

float FireTable[256][4]; // RGBA values instead of D3DCOLOR

struct FireParticle
{
	float x;
	float y;
	float c;
	float v;
};

FireParticle fireParticle[NUM_FIREPARTICLES];
FireParticle violetParticle[NUM_VIOLETPARTICLES];

#define NUM_PARTICLES 64

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

Vertex   g_Mesh[6];                 // Vertices used to render particles


int MIDASgetPosition(void)
{
  #ifdef MUSIC
    if (g_xmPlayer) {
      // Use the XM player to get the position
      uint32_t pos = xm_player_get_position(g_xmPlayer);
      VBLsync = xm_player_get_sync_info(g_xmPlayer);
      return pos;
    } else {
      // Fallback if player isn't initialized
      static int position = 0;
      static int syncCounter = 0;
      position += 1; // Increment position each call
      syncCounter = (syncCounter + 1) % 5; // Cycle sync counter 0-4
      VBLsync = syncCounter; // Update the sync info
      if (position > 0x3000) position = 0; // Reset after reaching end
      return position;
    }
	#else
    // Return a simulated position that cycles through different sections
    // This will allow the visual effects to cycle through different parts
    static int position = 0;
    static int syncCounter = 0;
    position += 1; // Increment position each call
    syncCounter = (syncCounter + 1) % 5; // Cycle sync counter 0-4
    VBLsync = syncCounter; // Update the sync info
    if (position > 0x3000) position = 0; // Reset after reaching end
    return position;
  #endif
}


void Rotozoom(char * map, float angle, float zoom, float xoffset, float yoffset, float alpha)
{
    #define PI 3.141592654

    g_pRenderer->BindTexture(g_pRenderer->LoadTexture(map));

    if (alpha == 1.0f) {
        g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
    } else {
        g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    }

    float ru[4];
    float rv[4];
    float z,a,x,y;

    z = zoom;
    a = angle+64+32;
    x = xoffset;
    y = yoffset;

    ru[2] = 3.5+z*cos(RAD(a-64.0))		+x;
    rv[2] = 3.5+z*sin(RAD(a-64.0))		+y;
    ru[0] = 3.5+z*cos(RAD(a+0.0))	+x;
    rv[0] = 3.5+z*sin(RAD(a+0.0))	+y;
    ru[1] = 3.5+z*cos(RAD(a+64.0))	+x;
    rv[1] = 3.5+z*sin(RAD(a+64.0))	+y;
    ru[3] = 3.5+z*cos(RAD(a+128.0))	+x;
    rv[3] = 3.5+z*sin(RAD(a+128.0))	+y;

    float c1[4], c2[4], c3[4], c4[4];

    a = 0.5+0.5*sin(RAD(VBLframe));
    a = alpha;

    c1[0] = 0.5+0.5*cos(RAD(VBLframe*0.27));
    c1[1] = 0.5+0.5*cos(RAD(VBLframe*0.96));
    c1[2] = 0.5+0.5*cos(RAD(VBLframe*7.26));
    c1[3] = a;

    c2[0] = 0.5+0.5*cos(RAD(VBLframe*0.02));
    c2[1] = 0.5+0.5*cos(RAD(VBLframe*0.94));
    c2[2] = 0.5+0.5*cos(RAD(VBLframe*0.34));
    c2[3] = a;

    c3[0] = 0.5+0.5*cos(RAD(VBLframe*5.62));
    c3[1] = 0.5+0.5*cos(RAD(VBLframe*4.26));
    c3[2] = 0.5+0.5*cos(RAD(VBLframe*0.43));
    c3[3] = a;

    c4[0] = 0.5+0.5*cos(RAD(VBLframe*0.71));
    c4[1] = 0.5+0.5*cos(RAD(VBLframe*2.65));
    c4[2] = 0.5+0.5*cos(RAD(VBLframe*0.81));
    c4[3] = a;

    // Set all colors to white for now
    c1[0] = c1[1] = c1[2] = c1[3] = 1.0f;
    c2[0] = c2[1] = c2[2] = c2[3] = 1.0f;
    c3[0] = c3[1] = c3[2] = c3[3] = 1.0f;
    c4[0] = c4[1] = c4[2] = c4[3] = 1.0f;

    Vertex grille[4];
    grille[0] = {g_pRenderer->GetWidth()/2.0f, g_pRenderer->GetHeight(), 0.99f, 1.0f, c1[0], c1[1], c1[2], c1[3], ru[0], rv[0]};
    grille[1] = {g_pRenderer->GetWidth()/2.0f, 0.0f, 0.99f, 1.0f, c2[0], c2[1], c2[2], c2[3], ru[1], rv[1]};
    grille[2] = {g_pRenderer->GetWidth(), g_pRenderer->GetHeight(), 0.99f, 1.0f, c3[0], c3[1], c3[2], c3[3], ru[2], rv[2]};
    grille[3] = {g_pRenderer->GetWidth(), 0.0f, 0.99f, 1.0f, c4[0], c4[1], c4[2], c4[3], ru[3], rv[3]};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
}

void DrawZoomSprite(char * sprite, float x1, float y1, float x2, float y2, float a, float z)
{
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pRenderer->BindTexture(g_pRenderer->LoadTexture(sprite));

    float u,v;

    u = 0.625/z/2.0;
    v = 0.9375/z/2.0;

    Vertex grille[4];
    grille[0] = {x1, y2, 1.0f, 1.0f, a, a, a, a, 0.3125f - u, 0.46875f + v};
    grille[1] = {x1, y1, 1.0f, 1.0f, a, a, a, a, 0.3125f - u, 0.46875f - v};
    grille[2] = {x2, y2, 1.0f, 1.0f, a, a, a, a, 0.3125f + u, 0.46875f + v};
    grille[3] = {x2, y1, 1.0f, 1.0f, a, a, a, a, 0.3125f + u, 0.46875f - v};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
    
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

void DrawAddSprite(char * sprite, float x1, float y1, float x2, float y2, float a1, float a2)
{
    static std::string lastSprite = "";
    if (std::string(sprite) != lastSprite) {
        std::cout << "DrawAddSprite: " << sprite << " (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ")" << std::endl;
        lastSprite = sprite;
    }

    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
    g_pRenderer->BindTexture(g_pRenderer->LoadTexture(sprite));

    Vertex grille[4];
    grille[0] = {x1, y2, 0.5f, 1.0f, a1, a1, a1, 1.0f, 0.0f, 1.0f};
    grille[1] = {x1, y1, 0.5f, 1.0f, a1, a1, a1, 1.0f, 0.0f, 0.0f};
    grille[2] = {x2, y2, 0.5f, 1.0f, a2, a2, a2, 1.0f, 1.0f, 1.0f};
    grille[3] = {x2, y1, 0.5f, 1.0f, a2, a2, a2, 1.0f, 1.0f, 0.0f};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
    
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

void DrawBackground(char * sprite, float zoom)
{
    static std::string lastSprite = "";
    if (std::string(sprite) != lastSprite) {
        std::cout << "DrawBackground: " << sprite << " zoom=" << zoom << std::endl;
        lastSprite = sprite;
    }

    float x1,y1,x2,y2;

    x1=g_pRenderer->GetWidth()/2.0f - zoom*(g_pRenderer->GetWidth()/2.0f);
    y1=g_pRenderer->GetHeight()/2.0f - zoom*(g_pRenderer->GetHeight()/2.0f);
    x2=g_pRenderer->GetWidth()/2.0f + zoom*(g_pRenderer->GetWidth()/2.0f);
    y2=g_pRenderer->GetHeight()/2.0f + zoom*(g_pRenderer->GetHeight()/2.0f);

    g_pRenderer->BindTexture(g_pRenderer->LoadTexture(sprite));
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

    Vertex grille[4];
    grille[0] = {x1, y2, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.9375f};
    grille[1] = {x1, y1, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};
    grille[2] = {x2, y2, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.625f, 0.9375f};
    grille[3] = {x2, y1, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.625f, 0.0f};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
    
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}


void DrawRect(float x1, float y1, float x2, float y2, float c)
{
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pRenderer->BindTexture(0); // No texture

    Vertex grille[4];
    grille[0] = {x1, y2, 1.0f, 1.0f, c, c, c, 1.0f, 0.0f, 1.0f};
    grille[1] = {x1, y1, 1.0f, 1.0f, c, c, c, 1.0f, 0.0f, 0.0f};
    grille[2] = {x2, y2, 1.0f, 1.0f, c, c, c, 1.0f, 1.0f, 1.0f};
    grille[3] = {x2, y1, 1.0f, 1.0f, c, c, c, 1.0f, 1.0f, 0.0f};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
    
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

void DrawAddRect(float x1, float y1, float x2, float y2, float c)
{
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
    g_pRenderer->BindTexture(0); // No texture

    Vertex grille[4];
    grille[0] = {x1, y2, 1.0f, 1.0f, c, c, c, 1.0f, 0.0f, 1.0f};
    grille[1] = {x1, y1, 1.0f, 1.0f, c, c, c, 1.0f, 0.0f, 0.0f};
    grille[2] = {x2, y2, 1.0f, 1.0f, c, c, c, 1.0f, 1.0f, 1.0f};
    grille[3] = {x2, y1, 1.0f, 1.0f, c, c, c, 1.0f, 1.0f, 0.0f};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
    
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

void DrawSubRect(float x1, float y1, float x2, float y2, float c)
{
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
    g_pRenderer->BindTexture(0); // No texture

    Vertex grille[4];
    grille[0] = {x1, y2, 1.0f, 1.0f, c, c, c, 1.0f, 0.0f, 1.0f};
    grille[1] = {x1, y1, 1.0f, 1.0f, c, c, c, 1.0f, 0.0f, 0.0f};
    grille[2] = {x2, y2, 1.0f, 1.0f, c, c, c, 1.0f, 1.0f, 1.0f};
    grille[3] = {x2, y1, 1.0f, 1.0f, c, c, c, 1.0f, 1.0f, 0.0f};

    std::vector<Vertex> vertices(grille, grille + 4);
    g_pRenderer->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertices);
    
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}


void Error(char *string, ... )
{
   char buf[256];

  va_list ap;
  va_start(ap, string);
  vsnprintf(buf, sizeof(buf), string, ap);
  va_end(ap);

#ifdef MUSIC
	MIDASclose();
#endif
  // In OpenGL version, we might want to use a different error reporting mechanism
  printf("[ERROR] %s\n", buf);
  exit(1);
}

void MIDASerror(void)
{
#ifdef MUSIC
    char        *errorMessage;

    errorMessage = MIDASgetErrorMessage(MIDASgetLastError());
    printf("MIDAS Error: %s\n", errorMessage);
    MIDASclose();
#endif

		exit(0);
}


//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit()
{
	int i;
	srand(0x9865485);

    // Load textures - they will be loaded on demand by the renderer

		#ifdef MUSIC
    MIDASstartup();
    // MIDASconfig();

		MIDASsetOption(MIDAS_OPTION_MIXING_MODE,MIDAS_MIX_HIGH_QUALITY);
		MIDASsetOption(MIDAS_OPTION_DSOUND_MODE, MIDAS_DSOUND_PRIMARY);

    if ( !MIDASinit() )
			MIDASerror();

    // Initialize the XM player with the embedded module data
    // The zik array contains the embedded XM module data from cochlea.xm.c
    g_xmPlayer = xm_player_init(zik, sizeof(zik), g_sample_rate);
    if (!g_xmPlayer) {
        // If XM player fails to initialize, continue without music
        printf("XM Player initialization failed, continuing without music\n");
    } else {
        xm_player_play(g_xmPlayer);
    }

		#endif

    for( i=0; i<NUM_FIREPARTICLES; i++ )
    {
			fireParticle[i].y=rand()%480;
			fireParticle[i].x=rand()%640;
			fireParticle[i].c=200+(rand()%50);
			fireParticle[i].v=(10+rand()%50)/30.0;
		}

    for( i=0; i<NUM_VIOLETPARTICLES; i++ )
    {
			violetParticle[i].y=rand()%480;
			violetParticle[i].x=rand()%200;
			violetParticle[i].c=8+(rand()%32);
			violetParticle[i].v=(10+(rand()%100))/50.0;
		}

    for( i=0; i<128; i++ ) {
        FireTable[i][0] = i/128.0;  // R
        FireTable[i][1] = 0.0;      // G
        FireTable[i][2] = 0.0;      // B
        FireTable[i][3] = 0.8;      // A
    }
    for( i=0; i<64; i++ ) {
        FireTable[128+i][0] = 1.0;      // R
        FireTable[128+i][1] = i/64.0;   // G
        FireTable[128+i][2] = 0.0;      // B
        FireTable[128+i][3] = 0.4;      // A
    }
    for( i=0; i<64; i++ ) {
        FireTable[128+i+64][0] = 1.0;       // R
        FireTable[128+i+64][1] = 1.0;       // G
        FireTable[128+i+64][2] = i/64.0;    // B
        FireTable[128+i+64][3] = 0.2;       // A
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove(FLOAT fTimeKey)
{
	static float old;
    static int logCounter = 0;

	VBLframe = 30.0*fTimeKey;

	VBLdelta = VBLframe-old;
	old=VBLframe;

	if (VBLdelta>100)
		VBLdelta=0;

    if (logCounter++ % 60 == 0) {
        std::cout << "Frame: " << VBLframe << " Delta: " << VBLdelta << " Song Pos: 0x" << std::hex << MIDASgetPosition() << std::dec << std::endl;
    }

	return S_OK;
}

void PartBegin()
{
		static float a=1.0;
		static float zt=1.0;
		static float z=0.0;
        static bool logged = false;
        if (!logged) { std::cout << "Entering PartBegin" << std::endl; logged = true; }

		if (MIDASgetPosition()>0x0420)
		{
			z = (MIDASgetPosition()&0xfc)-0x20;

			if (MIDASgetPosition()>=0x043b)
				z+=3;
			if (MIDASgetPosition()>=0x043c)
				z+=3;
			if (MIDASgetPosition()>=0x043e)
				z+=3;
		}


	if (MIDASgetPosition()>0x0200)
	{
		a-=VBLdelta*0.01;
		if (a<0)
			a=0;
		DrawBackground("mygirl.bmp", 1.0-a);
	}

	zt+=VBLdelta*0.001;

	if (MIDASgetPosition()<0x0300)
	{
		DrawAddSprite("title.bmp", g_pRenderer->GetWidth()/2.0f-256*zt, g_pRenderer->GetHeight()/2.0f-128*zt, g_pRenderer->GetWidth()/2.0f+256*zt, g_pRenderer->GetHeight()/2.0f+128*zt, a, a);
	}
	else
		DrawBackground("mygirl.bmp", 1.0+z/32.0);


	int y;
	if (MIDASgetPosition()>0x0200)
	for(y=0;y<8;y++)
	{
		static float x;
		if (VBLsync==1)
		{
//			x+=VBLdelta/2.0;
//			if(x>40.0)
				x=40.0;
		}
		x-=VBLdelta/4.0;
		if(x<0.0)
			x=0.0;

		int i;
		i = 128*y+VBLframe;
		i = i&1023;
		i = 512-i;
		// Original D3D used two distinct horizontal anchors (384 and 640 on a
		// 640-wide screen) so each logo spans ~256px on the right of the frame.
		// Scale them by GetWidth()/640 for resolution independence (same
		// convention as PartPhantasm's GetWidth()*476/640).
		DrawAddSprite("title.bmp", g_pRenderer->GetWidth()*384/640.0f-32-x, i, g_pRenderer->GetWidth()-32+x, i+128, 1.0-a, 0.0);
		i = -i;
		DrawAddSprite("title.bmp", g_pRenderer->GetWidth()*384/640.0f-32+x, i, g_pRenderer->GetWidth()-32-x, i+128, 0.0, 1.0-a);
	}

}

void PartMyGirl()
{
	float x,y;
	static float dist,angle;
	static float speed;
	static float pos;

	int n = (MIDASgetPosition()&255);
	switch (n)
	{
		case 0:
		case 3:
		case 6:
		case 8:
		case 11:
		case 14:
		case 16:
		case 17:
		case 19:
		case 22:
		case 24:
		case 27:
		case 30:
		case 32:
		case 35:
		case 38:
		case 40:
		case 43:
		case 46:
		case 48:
		case 49:
		case 51:
		case 54:
		case 56:
		case 57:
		case 59:
		case 62:
			speed+= VBLdelta;
			break;
		default:
			speed-= VBLdelta*2.0;
	}

	if (speed<1.0)
		speed = 1.0;

//	pos+=VBLdelta*speed;
	pos+=VBLdelta;

	x = -g_pRenderer->GetWidth()/2.0f - g_pRenderer->GetWidth()/2.0f*sin(0.012*pos);
	y = -g_pRenderer->GetHeight()/2.0f - g_pRenderer->GetHeight()/2.0f*cos(0.021*pos);

	DrawAddSprite("mygirl.bmp", x, y, 2*1024+x, 2*512+y, 1.0, 1.0);


	dist = 1+VBLsync;

	if ((MIDASgetPosition()>>8)>=9)
		angle+=VBLdelta;
	else
	{
		if (VBLsync==4)
			angle = 80.0;
		else
			angle = 50.0;
	}

	// Turn on alpha blending for the particles
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->BindTexture(g_pRenderer->LoadTexture("light.bmp"));

    // Do the particles
    for( float j=0; j<4; j++ )
    for( float i=0; i<128; i++ )
    {
			float x,y,z,r,a;
			r = angle+1*i;
			a = 64*j+1*VBLframe+i*(dist*sin(RAD(1.0*VBLframe)));  //8.0;
			x = g_pRenderer->GetWidth()/2.0f + r*sin(RAD(a));
			y = g_pRenderer->GetHeight()/2.0f + r*cos(RAD(a));
			z = 25.0+10.0*sin(RAD(2.0*i+VBLframe));
			
			float c[4];
			c[0] = 0.5+0.5*sin(RAD(0.21*VBLframe+4.98*i));
			c[1] = 0.5+0.5*sin(RAD(3.12*VBLframe+1.28*i));
			c[2] = 0.5+0.5*sin(RAD(7.94*VBLframe+2.17*i));
			c[3] = 0.5+0.3*sin(RAD(1.19*VBLframe+3.14*i));

			g_Mesh[0] = {x-z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 1.0f};
			g_Mesh[1] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
			g_Mesh[2] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
			g_Mesh[4] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
			g_Mesh[3] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
			g_Mesh[5] = {x+z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 0.0f};

            std::vector<Vertex> vertices(g_Mesh, g_Mesh + 6);
            g_pRenderer->DrawPrimitive(D3DPT_TRIANGLELIST, vertices);
    }
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

void PartScroll()
{
	int n,i;
	static float y=500;
	static float z=1.0;
	static float a=0.0;

	if (z>10)
		a+=VBLdelta*0.02;

	if (a>1.0)
		a = 1.0;

	DrawBackground("mygirl.bmp", z);

	DrawAddSprite("title.bmp", g_pRenderer->GetWidth()/4.0f-128, g_pRenderer->GetHeight()/2.0f-64, g_pRenderer->GetWidth()/4.0f+128, g_pRenderer->GetHeight()/2.0f+64, a, a);
	// Original D3D centred pub.bmp on y=300 (=H/2+60) with a half-height of 16,
	// i.e. y-span [284,316]. The earlier port used +60/+92, which put 300 at the
	// top edge and shifted the banner 16px down - centre it on H/2+60 instead.
	DrawAddSprite("pub.bmp", g_pRenderer->GetWidth()/4.0f-128, g_pRenderer->GetHeight()/2.0f+44, g_pRenderer->GetWidth()/4.0f+128, g_pRenderer->GetHeight()/2.0f+76, a, a);

	if (y<-1200)
		// Signal main()'s loop to stop, instead of calling exit() directly
		// here - that used to skip the audio device / module / GL context
		// cleanup that only runs on a normal loop exit.
		g_demoFinished = true;

	if (VBLdelta<10)
	{
		y -= 1.0*VBLdelta;
		if (z<20.0)
			z *= 1.0+0.003*VBLdelta;
	}

	n = 20.0*sin(VBLframe*0.1);

	DrawAddSprite("scroll.bmp", g_pRenderer->GetWidth()/2.0f, y, g_pRenderer->GetWidth()/2.0f+256, y+1024, 0.8, 0.8);

	for(i=2;i<5;i+=1)
		DrawAddSprite("scroll.bmp", g_pRenderer->GetWidth()/2.0f+160-128*i, y*i, g_pRenderer->GetWidth()/2.0f+160+128*i, y+1024*i, 0.5/i, 0.5/i);
}

struct Flare
{
	float x,y,p;
};

#define NUM_FLARES 13

Flare flares[NUM_FLARES]={{507,133},{525,117},{595,75},{326,116},{297,141},{218,169},{315,5},{311,56},{308,22},{489,148},{570,80},{290,127},{308,136}};


void PartFire()
{
	static Flare flare={0,0,1024};

	DrawBackground("knights.bmp", 1.0);

	int n;

	for(n=0;n<NUM_FLARES;n++)
	{
			int x,y,z;
			x = flares[n].x;
			y = flares[n].y;
			z = 16.0*sin((125.2567*n+4.0*VBLframe)*3.1415927*2.0/256.0)+16.0*sin((13.259*n+1.0*VBLframe)*3.1415927*2.0/256.0);
			if (z)
				DrawAddSprite("flare.bmp", x-z, y-z, x+z, y+z, z/32.0, z/32.0);
	}

    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

    g_pRenderer->BindTexture(g_pRenderer->LoadTexture("anim.bmp"));

    // Do the particles
    for( int i=0; i<NUM_FIREPARTICLES; i++ )
    {
			float c[4];
			float x,y,z;

			fireParticle[i].y-=fireParticle[i].v*VBLdelta*2.0;
			fireParticle[i].c-=fireParticle[i].v*VBLdelta*0.8*2.0;

			if (fireParticle[i].y <= 200.0)
			{
				fireParticle[i].c=100+(rand()%150);
				fireParticle[i].y=480+32/*+(rand()%50)*/;
				fireParticle[i].x=rand()%640;
				fireParticle[i].v=(10+(rand()%100))/50.0;
			}

			x = fireParticle[i].x;
			y = fireParticle[i].y;
			z = fireParticle[i].c/2.0;

			if (fireParticle[i].c > 10.0)
			{
				c[0] = FireTable[((int)fireParticle[i].c)&255][0];
				c[1] = FireTable[((int)fireParticle[i].c)&255][1];
				c[2] = FireTable[((int)fireParticle[i].c)&255][2];
				c[3] = FireTable[((int)fireParticle[i].c)&255][3];

				float u1,u2,v1,v2;
				int f = VBLframe+i+fireParticle[i].c;
				int mx,my;

				mx = 32*(f&7);
				my = 32*((f>>3)&3);

				u1 = (mx+1)/256.0;
				v1 = (my+1)/128.0;

				u2 = (mx+30)/256.0;
				v2 = (my+30)/128.0;

				g_Mesh[0] = {x-z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u1, v2};
				g_Mesh[1] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u1, v1};
				g_Mesh[2] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u2, v2};
				g_Mesh[4] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u1, v1};
				g_Mesh[3] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u2, v2};
				g_Mesh[5] = {x+z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u2, v1};

                std::vector<Vertex> vertices(g_Mesh, g_Mesh + 6);
                g_pRenderer->DrawPrimitive(D3DPT_TRIANGLELIST, vertices);
			}
    }

	if (MIDASgetPosition()>0x1300)
	{
		int x=0,y=0;
		static float z,a;

		static float pos=0;
		static int fade=0;

		pos+=VBLdelta*0.01;

		if (pos>=1.0)
		{
			pos=0.0;
			fade++;
			if (fade==3)
				fade = 0;
		}

		a = pos;
		z = 1.0+ 1.0*pos;

		if (fade == 0)
		{
			DrawZoomSprite("phantasm.bmp", x, y, x+160, y+120, 1.0-a, 2.0*z);
			DrawZoomSprite("mygirl.bmp", x, y, x+160, y+120, a, z);
		}
		if (fade == 1)
		{
			DrawZoomSprite("mygirl.bmp", x, y, x+160, y+120, 1.0-a, 2.0*z);
			DrawZoomSprite("knights.bmp", x, y, x+160, y+120, a, z);
		}
		if (fade == 2)
		{
			DrawZoomSprite("knights.bmp", x, y, x+160, y+120, 1.0-a, 2.0*z);
			DrawZoomSprite("phantasm.bmp", x, y, x+160, y+120, a, z);
		}
	}
}

void PartPhantasm()
{
	DrawBackground("phantasm.bmp", 1.0);

	static float flare=0.0;

	if( (MIDASgetPosition()&31)==0)
		flare=32.0;

	flare-=VBLdelta*0.5;

	if (flare<0)
		flare=0;
	else
	 DrawAddSprite("flare.bmp", g_pRenderer->GetWidth()*476/640-flare, g_pRenderer->GetHeight()*207/480-flare, g_pRenderer->GetWidth()*476/640+flare, g_pRenderer->GetHeight()*207/480+flare, flare*32.0, flare*32.0);

    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

	int x;
	int y;
	static float power=0.0;
	static float alpha=0.0;

	if( (MIDASgetPosition()>=0x1d00) )
	{
		power+=VBLdelta*0.5;
		alpha+=VBLdelta*0.002;
		if (alpha>0.7)
			alpha=0.7;
	}

	if ((GetAsyncKeyState(VK_UP)&0xf000))
		power+=0.1;
	if ((GetAsyncKeyState(VK_DOWN)&0xf000))
		power-=0.1;

	if ((GetAsyncKeyState(VK_RIGHT)&0xf000))
		alpha+=0.01;
	if ((GetAsyncKeyState(VK_LEFT)&0xf000))
		alpha-=0.01;

	if (power<0)
		power = 0;

	if (alpha<0)
		alpha = 0;
	if (alpha>1.0)
		alpha = 1.0;

	Vertex line[2*40*40];
	float c[4] = {alpha, alpha, alpha, 1};

	for(y=-20;y<20;y++)
		for(x=-20;x<20;x++)
		{
			float i,j;

			i = x*40+400.0*sin((x+2*y+VBLframe)*2.0*3.1415927/256.0)*sin((7*y+5*VBLframe)*2.0*3.1415927/256.0);
			j = y*40+300.0*cos((3*x+y+VBLframe)*2.0*3.1415927/256.0)*sin((5*x+3*VBLframe)*2.0*3.1415927/256.0);

			float u1,u2,v1,v2;

			u1=0;
			v1=(-VBLframe)*0.01/64.0;
			u2=1.0;
			v2=v1+power/64.0;

			line[(y+20)*80+(x+20)*2+0] = {g_pRenderer->GetWidth()*476/640, g_pRenderer->GetHeight()*207/480, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u1, v1};
			line[(y+20)*80+(x+20)*2+1] = {g_pRenderer->GetWidth()/2.0f+i, g_pRenderer->GetHeight()/2.0f+j, 0.5f, 0.5f, c[0], c[1], c[2], c[3], u2, v2};
		}

		g_pRenderer->BindTexture(g_pRenderer->LoadTexture("line.bmp"));
		if (alpha>0) {
            std::vector<Vertex> vertices(line, line + 2*40*40);
            g_pRenderer->DrawPrimitive(D3DPT_LINELIST, vertices);
        }

    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

    g_pRenderer->BindTexture(g_pRenderer->LoadTexture("violet.bmp"));

			static float violet=0.0;
			{
				violet += VBLdelta*0.01;
				if (violet > 1.0)
					violet = 1.0;
			}

			if( (MIDASgetPosition()>=0x1d00) )
			{
				violet -= VBLdelta*0.02;
				if (violet<0.0)
					violet = 0.0;
			}

    // Do the particles
    for( int i=0; i<NUM_VIOLETPARTICLES; i++ )
    {
			float c[4];
			float x,y,z;

			violetParticle[i].y-=violetParticle[i].v*VBLdelta;

			if (violetParticle[i].y <= -40.0)
			{
				if (i&1)
				  violetParticle[i].c=8+(rand()%32);
				else
				  violetParticle[i].c=8+(rand()%8);
				violetParticle[i].y=480+32+(rand()%50);
				violetParticle[i].x=40+(rand()%160);
				violetParticle[i].v=(30+(rand()%80))/20.0;   //violetParticle[i].c;
			}

			static float a=0.0;

			x = 120.0+80.0*sin((violetParticle[i].x+violetParticle[i].y+3.0*sin(VBLframe*0.01))*3.1415927*2.0/300.0);
			y = violetParticle[i].y;
			z = violetParticle[i].c;

			if (violet>0)
			{
				c[0] = c[1] = c[2] = violet;
                c[3] = 1.0;

				g_Mesh[0] = {x-z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 1.0f};
				g_Mesh[1] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
				g_Mesh[2] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
				g_Mesh[4] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
				g_Mesh[3] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
				g_Mesh[5] = {x+z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 0.0f};

                std::vector<Vertex> vertices(g_Mesh, g_Mesh + 6);
                g_pRenderer->DrawPrimitive(D3DPT_TRIANGLELIST, vertices);
			}
    }

		if (violet>0)
			for (int n=0;n<2048;n+=100)
			{
				int y = 1024-((n+(int)VBLframe*5)&2047);
				DrawAddSprite("light.bmp", 120-16, y-16, 120+16, y+16, 1.0, 1.0);
			}

    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

void PartPurple()
{
  g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
  g_pRenderer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
  g_pRenderer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

	int x;
	int y;
	static float power=1.0;
	static float alpha=0.0;
	static float alphamap=0.0;
	static float dist=200.0;

	power = sin(VBLframe*0.02);

	if (MIDASgetPosition()>0x1600)
	{
		alpha+=VBLdelta*0.03;
		if (alpha>1.0)
			alpha = 1.0;

		dist -= VBLdelta;
		if (dist<20.0)
			dist = 20.0;
	}
	if (MIDASgetPosition()>0x1700)
	{
		alphamap+=VBLdelta*0.04;
		if (alphamap>1.0)
			alphamap = 1.0;
	}

	if ((GetAsyncKeyState(VK_UP)&0xf000))
		power+=0.01;
	if ((GetAsyncKeyState(VK_DOWN)&0xf000))
		power-=0.01;

	if ((GetAsyncKeyState(VK_RIGHT)&0xf000))
		alpha+=0.01;
	if ((GetAsyncKeyState(VK_LEFT)&0xf000))
		alpha-=0.01;

	if (alpha<0)
		alpha = 0;
	if (alpha>1.0)
		alpha = 1.0;

	Vertex line[2*40*40];

	float oi,oj;

	int r,a,n=0;

	for(y=0;y<480;y+=20)
	{
		int y1,y2;

		y2 = y+30.0+10.0*sin((VBLframe+y)*0.1);

		float c[4];

		n++;
		if (n&1) {
            c[0] = 0.42*0.3; c[1] = 0.31*0.3; c[2] = 0.19*0.3; c[3] = 1.0;
        }
		else {
            c[0] = 0.35*0.3; c[1] = 0.19*0.3; c[2] = 0.07*0.3; c[3] = 1.0;
        }

		g_Mesh[0] = {0, y2, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 1.0f};
		g_Mesh[1] = {0, y, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
		g_Mesh[2] = {g_pRenderer->GetWidth(), y2, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
		g_Mesh[3] = {0, y, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
		g_Mesh[4] = {g_pRenderer->GetWidth(), y2, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
		g_Mesh[5] = {g_pRenderer->GetWidth(), y, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 0.0f};

        std::vector<Vertex> vertices(g_Mesh, g_Mesh + 6);
        g_pRenderer->DrawPrimitive(D3DPT_TRIANGLELIST, vertices);
	}

for(n=0;n<4;n++)
{
	oi=oj=-100;
	for(y=0;y<40;y++)
		for(a=0;a<40;a++)
		{
			float i,j,r,z;

			r = 40.0+10.0*sin((VBLframe+a*40)*3.1415927*2.0/400.0)*cos((-VBLframe+a*5)*3.1415927*2.0/400.0);
			r += 10.0*sin((VBLframe+y*10)*3.1415927*2.0/400.0);

			j = -50+y*dist+power*r*sin((10*a+VBLframe)*3.1415927*2.0/400.0);
			i = (4.0+n)*r*cos((10*a+-VBLframe)*3.1415927*2.0/400.0);

			j += a*dist/40.0;

			float c[4] = {alpha*(1.0-n/4.0), alpha*(1.0-n/4.0), alpha*(1.0-n/4.0), 1.0};

			i += 2.0*(y-20)*sin(VBLframe*0.02);

			line[y*80+a*2+0] = {g_pRenderer->GetWidth()/2.0f+oi, oj, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.3f, 0.3f};
			line[y*80+a*2+1] = {g_pRenderer->GetWidth()/2.0f+i, j, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.7f, 0.7f};

			oi=i;
			oj=j;
		}

	g_pRenderer->BindTexture(g_pRenderer->LoadTexture("mygirl.bmp"));

	if (n==0)
	for(y=1;y<40;y++)
		for(a=1;a<40;a++)
		{
			float a1=alphamap,z=40.0;
			float x1,y1,x2,y2;

			line[y*80+a*2+0];

			g_Mesh[0] = line[(y-1)*80+(a-1)*2+1];
			g_Mesh[0].tu=(a)/z;
			g_Mesh[0].tv=(y)/z;
			g_Mesh[0].r = g_Mesh[0].g = g_Mesh[0].b = a1;
            g_Mesh[0].a = 1.0;

			g_Mesh[1] = line[(y-1)*80+(a)*2+1];
			g_Mesh[1].tu=(a+1)/z;
			g_Mesh[1].tv=(y)/z;
			g_Mesh[1].r = g_Mesh[1].g = g_Mesh[1].b = a1;
            g_Mesh[1].a = 1.0;

			g_Mesh[2] = line[(y)*80+(a-1)*2+1];
			g_Mesh[2].tu=(a)/z;
			g_Mesh[2].tv=(y+1)/z;
			g_Mesh[2].r = g_Mesh[2].g = g_Mesh[2].b = a1;
            g_Mesh[2].a = 1.0;

			g_Mesh[3] = line[(y)*80+(a-1)*2+1];
			g_Mesh[3].tu=(a)/z;
			g_Mesh[3].tv=(y+1)/z;
			g_Mesh[3].r = g_Mesh[3].g = g_Mesh[3].b = a1;
            g_Mesh[3].a = 1.0;

			g_Mesh[4] = line[(y)*80+(a)*2+1];
			g_Mesh[4].tu=(a+1)/z;
			g_Mesh[4].tv=(y+1)/z;
			g_Mesh[4].r = g_Mesh[4].g = g_Mesh[4].b = a1;
            g_Mesh[4].a = 1.0;

			g_Mesh[5] = line[(y-1)*80+(a)*2+1];
			g_Mesh[5].tu=(a+1)/z;
			g_Mesh[5].tv=(y)/z;
			g_Mesh[5].r = g_Mesh[5].g = g_Mesh[5].b = a1;
            g_Mesh[5].a = 1.0;

            std::vector<Vertex> vertices(g_Mesh, g_Mesh + 6);
            g_pRenderer->DrawPrimitive(D3DPT_TRIANGLELIST, vertices);
		}

	  g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
		g_pRenderer->BindTexture(g_pRenderer->LoadTexture("light.bmp"));
		std::vector<Vertex> lineVertices(line, line + 2*40*40);
		g_pRenderer->DrawPrimitive(D3DPT_LINELIST, lineVertices);
}
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
}

//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render()
{
    g_pRenderer->Clear(0.0f, 0.0f, 0.0f, 1.0f); // Clear to black

    g_pRenderer->BeginScene();

    uint32_t pos = MIDASgetPosition();
    static uint32_t lastLoggedPos = 0xFFFFFFFF;
    if (pos >> 8 != lastLoggedPos >> 8) {
        std::cout << "App_Render: Song Pos 0x" << std::hex << pos << std::dec << std::endl;
        lastLoggedPos = pos;
    }

    if( (pos>=0x0000) && (pos<0x0500) )
		PartBegin();
	else if( (pos>=0x0500) && (pos<0x0a00) )
		PartMyGirl();
	else if( (pos>=0x0a00) && (pos<0x1500) )
		PartFire();
	else if( (pos>=0x1500) && (pos<0x1900) )
		PartPurple();
	else if( (pos>=0x1900) && (pos<0x1f00) )
		PartPhantasm();
	else if( (pos>=0x2100) && (pos<0x3000) )
		PartScroll();
    else {
        static int skipCounter = 0;
        if (skipCounter++ % 60 == 0) {
            std::cout << "No part for position 0x" << std::hex << pos << std::dec << std::endl;
        }
    }

    g_pRenderer->EndScene();
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderParticles()
// Desc: Draws the system of particles
//-----------------------------------------------------------------------------
HRESULT RenderParticles()
{
	// Turn on alpha blending for the particles
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pRenderer->BindTexture(g_pRenderer->LoadTexture("light.bmp"));

    // Do the particles
    for( float i=0; i<NUM_PARTICLES; i++ )
    {
			float x,y,z,r,a;
			r = 40+2*i;
			a = 10*VBLframe+i*(5.0*sin(RAD(3.0*VBLframe)));  //8.0;
			x = g_pRenderer->GetWidth()/2.0f + r*sin(RAD(a));
			y = g_pRenderer->GetHeight()/2.0f + r*cos(RAD(a));
			z = 25.0+10.0*sin(RAD(5.0*i+VBLframe));
			
			float c[4];
			c[0] = 0.5+0.5*sin(RAD(0.21*VBLframe+4.98*i));
			c[1] = 0.5+0.5*sin(RAD(3.12*VBLframe+1.28*i));
			c[2] = 0.5+0.5*sin(RAD(7.94*VBLframe+2.17*i));
			c[3] = 1.0; // Alpha is 1

			g_Mesh[0] = {x-z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 1.0f};
			g_Mesh[1] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
			g_Mesh[2] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
			g_Mesh[4] = {x-z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 0.0f, 0.0f};
			g_Mesh[3] = {x+z, y+z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 1.0f};
			g_Mesh[5] = {x+z, y-z, 0.5f, 0.5f, c[0], c[1], c[2], c[3], 1.0f, 0.0f};

            std::vector<Vertex> vertices(g_Mesh, g_Mesh + 6);
            g_pRenderer->DrawPrimitive(D3DPT_TRIANGLELIST, vertices);
    }
    g_pRenderer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects()
{
    // Set up the dimensions for the background image
    // In OpenGL, we don't need to create materials like in Direct3D

    // Set up the orthographic projection
    // This is handled in the renderer initialization

    // Set any appropriate state
    // These are handled by the renderer

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup()
{
    App_DeleteDeviceObjects();

		#ifdef MUSIC
    /* Uninitialization */

    /* Remove music sync callback: */
    if ( !MIDASsetMusicSyncCallback(playHandle, NULL) )
        MIDASerror();

    /* Stop playing module: */
    if ( !MIDASstopModule(playHandle) )
        MIDASerror();

		MIDASstopBackgroundPlay();

    /* Deallocate the module: */
    if ( !MIDASfreeModule(module) )
        MIDASerror();

    /* Close MIDAS: */
    if ( !MIDASclose() )
        MIDASerror();

		// Clean up the XM player
		if (g_xmPlayer) {
			xm_player_stop(g_xmPlayer);
			xm_player_destroy(g_xmPlayer);
			g_xmPlayer = nullptr;
		}
		#endif
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
void App_DeleteDeviceObjects()
{
    // In OpenGL, textures are managed by the renderer
}

//-----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//-----------------------------------------------------------------------------
HRESULT App_RestoreSurfaces()
{
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice()
{
    // In OpenGL, we don't need to check device capabilities like in Direct3D
    return S_OK;
}