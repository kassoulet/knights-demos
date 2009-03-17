#include <mikmod.h>
#include <stdio.h>


MODULE *module;

int MusicGetPosition(void)
{
  return 256 * module->sngpos + module->patpos;
}

void MusicSetPosition(int position)
{
  Player_SetPosition(position);
}

int MusicUpdate()
{
  if (Player_Active()) {
    MikMod_Update();
  }
  return 1;
}

void MusicStart(char *filename)
{
md_mixfreq = 44100;
md_device   = 0;
md_volume   = 128;
md_musicvolume = 128;
md_sndfxvolume = 128;
md_pansep   = 128;
md_reverb   = 0;

  MikMod_RegisterAllLoaders();
  MikMod_RegisterAllDrivers();

  MikMod_Init(NULL);
   
  module = Player_Load(filename, 32, 0);
  if (module == NULL)
  {
    printf("Error loading %s", filename);
    return;
  }
  Player_Start(module);
  module->loop = 1; // allows module to loop at the end.
}


void MusicStop()
{
  Player_Stop();
  MikMod_Exit(); 
}



/*
void OpenSystem()
{
        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
                fprintf(stderr,"Couldn't initialize SDL: %s\n",SDL_GetError());
                        exit( 1 );
        }
}

SDL_Quit();

MODULE *module;
SDL_RWops *op;
void * module_data;
int module_size;

#define FREQ 44100
#define BUFFERSIZE 2048
#define SAMPLESIZE 16
#define INTERPOLATION 1

void SDLMikMod_FillAudio(void *udata, Uint8 *stream, int len)
{
        if (Player_Active()) {
                VC_WriteBytes((Sint8 *)stream, len);
        } else {
                VC_SilenceBytes((Sint8 *)stream, len);
        }
}

void OpenMusic(void * pModule, int size)
{
        SDL_AudioSpec wanted;
        SDL_AudioSpec fmt;

        wanted.freq = FREQ;
	
        // 16bits or 8 bits ?
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

        if ((fmt.format&0xFF)==8) {
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
	
        if (!module) {
                fputs("error loading module!",stderr);
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
        return 256*pattern+row;
}

void SetMusicPosition(int pattern)
{
        Player_SetPosition(pattern);
}
*/



