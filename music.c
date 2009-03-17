#include "mikmod.h"
#include "SDL.h"

#define FREQ 44100
#define BUFFERSIZE 2048
#define SAMPLESIZE 16
#define INTERPOLATION 1
MODULE *module;

Uint32 MusicUpdate(Uint32 interval, void *param)
{
  MikMod_Update();
  return interval;
}

void OpenMusic(char* filename)
{
  md_mixfreq = FREQ;
  md_device   = 0;
  md_volume   = 128;
  md_musicvolume = 128;
  md_sndfxvolume = 128;
  md_pansep   = 128;
  md_reverb   = 0;

  MikMod_RegisterAllLoaders();
  MikMod_RegisterAllDrivers();

    if (MikMod_Init("")) {
        fprintf(stderr, "Could not initialize sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return;
    }
  
  module = Player_Load(filename, 32, 0);
  Player_Start(module);
  
  SDL_AddTimer(20, MusicUpdate, 0);
  
  if (!module) {
    fputs("error loading module!",stderr);
    exit(1);
    return;
  }
}

int GetPosition(void)
{
    
  int pattern, row;
  return module->sngpos*256 + module->patpos;
}

void CloseMusic()
{
  Player_Stop();
  MikMod_Exit();
}
