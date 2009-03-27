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

init = 0;

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
    //md_mode |= DMODE_HQMIXER;
    md_mode |= DMODE_16BITS;

    if(!init) {
        MikMod_RegisterAllLoaders();
        MikMod_RegisterAllDrivers();
        init = 1;
    }

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

void MusicDump(char *filename)
{
    puts("dumping music...");
    md_device = MikMod_DriverFromAlias("wav");
    MusicStart(filename);
    module->loop = 0;
    while (Player_Active())
    {
        MikMod_Update();
    }
    MusicStop();
}


