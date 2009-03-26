#include <mikmod.h>
#include <stdio.h>

/*
*/

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

    md_mode |= DMODE_INTERP;
    md_mode |= DMODE_HQMIXER;
    md_mode |= DMODE_16BITS;

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
    module->loop = 0; // allows module to loop at the end.
}


void MusicStop()
{
    Player_Stop();
    MikMod_Exit();
}



