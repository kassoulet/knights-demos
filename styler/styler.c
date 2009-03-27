/*

  Styler

  Coded by Gautier "Impulse" PORTET <kassoulet gmail com>

  01/98  02/98

  (c) 1998-2002 KNIGHTS

  Main File

*/

#define USE_CONSOLE

#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "styler.h"
#include "k3d.h"
#include "effects.h"
#include <SDL.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT= 400;

float SCALE_X;
float SCALE_Y;

int mode=GFX_AUTODETECT_WINDOWED;
// globals

extern int MusicGetPosition(void);
extern void MusicStart(char *filename);
extern void MusicStop();
extern int MusicUpdate();
extern void MusicSetPosition(int position);

int quit = 0;
int GetPosition(void)
{
    if (key[KEY_ESC])
        quit = 1;
    if (quit)
        return 0xffff;
    if (key[KEY_N])
        return 0xffff;
    return MusicGetPosition();
}

BITMAP * buffer;
DATAFILE *data;

int VBLframe,VBLfps,VBLcount,VBLold;
int refreshRate;

COLOR_MAP tableGlenz1,tableGlenz2,tableGlenz3,tableBalls;
COLOR_MAP light1,light2;
COLOR_MAP tableEye;

K3DTObject cube;
K3DTObject patate;
K3DTObject aim;
K3DTObject city;
K3DTObject bizar;
K3DTObject bizar2;

unsigned short *lookupDeep;
unsigned short *lookupFlat;
unsigned short *lookupDeep2;
unsigned short *lookupFlat2;

RLE_SPRITE * counter[10];
PALETTE head_pal;

void Fatal(char* str)
{
    printf("\nFatal : %s\n", str);
    exit(0);
}

/*

void TimerHandler(void);


void TimerHandler(void)
{
    VBLfps = VBLcount;
    VBLcount = 0;
}



void WaitVBL()
{
    VBLframe = retrace_count;
    VBLcount++;

    if (key[KEY_TILDE])
    {
        BITMAP * display;
        char buf[16];
        int i;

        display = create_bitmap(8*16, 10);
        clear_to_color(display, 0);
        buf[0] = 0;
        sprintf(buf, " Styler %3d FPS", VBLfps);


        i = 8*16*VBLfps/70;
        if (i >= 8*16-1)
            i = 8*16-2;
        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

        rect(display, 0, 0, 8*16-1, 9, 80);
        rectfill(display, 1, 1, i, 8, 50);
        text_mode(-1);
        textout(display, font, buf, 1, 1, 127);
        blit(display, buffer, 0, 0, 0, 0, 8*16, 10);
        blit(display, screen, 0, 0, 0, 0, 8*16, 10);
    }

}
*/

Uint32 TimerHandler(Uint32 interval, void *param)
{
    VBLfps = VBLcount;
    VBLcount = 0;
    //printf("fps: %d\n", VBLfps);
    return interval;
}

Uint32 TimerHandler2(Uint32 interval, void *param)
{
    VBLframe++;
    return interval;
}
static Uint32 fps_count=0;
static Uint32 fps_frames=0;
Uint32 ticks=0;
double delta;
Uint32 frames=0;
double timer=0;
double oldTicks=0;
double fps;

static void UpdateTimer()
{
    Uint32 diff;
    ticks = SDL_GetTicks();
    diff = (ticks-oldTicks);

    fps_count += diff;
    fps_frames++;
    if (fps_count > 1000)
    {
        fps = fps_frames * fps_count / 1000.0;
        fps_count = fps_frames = 0;
    }

    /* we interpolate delta to get a smooth movement */
    delta = delta*0.5 + (diff/20.0)*0.5;

    frames++;
    VBLcount++;
    timer+=delta;

    VBLframe = timer;
    //VBLdelta = delta;
    //fVBLdelta= delta;
    oldTicks = ticks;
}

void WaitVBL()
{
    UpdateTimer();
    
    if (key[KEY_SPACE])
    {
        BITMAP * display;
        char buf[16];
        int i,j;

        if (key[KEY_P])
        {
            //
            // save_bitmap("dump.tga", buffer, pal);
        }

        display = create_bitmap(8*16, 10);
        clear_to_color(display, 0);

        buf[0] = 0;
        if ( fps > 10000 || fps < 0)
        {
            strcpy(buf, "error");
        }
        else
        {
            sprintf(buf, "%.2f FPS", fps);
        }
        i = 8*16*2*fps/70;
        if (i >= 8*16-1)
            i = 8*16-2;
        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

        for (j=1;j<=i;j++)
        {
            vline(display, j, 0, 8, 63);
        }

        text_mode(-1);
        textout(display, font, buf, 1, 1, makecol(250,250,250));

        blit(display, buffer, 0, 0, 0, 0, 8*16, 9);

        destroy_bitmap(display);

    }    
}

void DrawCounter(void)
{
    draw_trans_rle_sprite(buffer, counter[(VBLframe/3)%9], SCREEN_W-1-92, 0);
}

void Update()
{
    WaitVBL();
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void InitDemo(void)
{
    int i;
    PALETTE pal;

    SCALE_X = SCREEN_W/320.0;
    SCALE_Y = SCREEN_H/200.0;    

    counter[0] = data[Bn0].dat;
    counter[1] = data[Bn1].dat;
    counter[2] = data[Bn2].dat;
    counter[3] = data[Bn3].dat;
    counter[4] = data[Bn4].dat;
    counter[5] = data[Bn5].dat;
    counter[6] = data[Bn6].dat;
    counter[7] = data[Bn7].dat;
    counter[8] = data[Bn8].dat;
    counter[9] = data[Bn9].dat;

    memcpy(pal, data[Phead].dat, sizeof (pal));
    for (i = 0;i < 128;i++)
    {
        head_pal[i].r = pal[i].r;
        head_pal[i].g = pal[i].g;
        head_pal[i].b = pal[i].b;
        head_pal[i+128].r = pal[i+128].b;
        head_pal[i+128].g = pal[i+128].r;
        head_pal[i+128].b = pal[i+128].g;
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>9))/1;
        if (c > 127)
            c = 127;
        tableGlenz1.data[i>>8][i&255] = c;
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>10));
        if (c > 127)
            c = 127;
        tableGlenz2.data[i>>8][i&255] = c;
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>11))/1;
        if (c > 127)
            c = 127;
        tableGlenz3.data[i>>8][i&255] = c;
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>9));
        if (c > 255)
            c = 255;
        tableBalls.data[i>>8][i&255] = c;
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;

        c = ((i&255)*(i>>8))/256;
        light1.data[i>>8][i&255] = c;
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;

        c = ((i&255)+(i>>8))/2;
        if (c > 127)
            c = 127;
        light2.data[i>>8][i&255] = c;
    }

    create_light_table(&tableEye, data[Pend].dat, 0, 0, 0, NULL);
    printf(".");
    fflush(stdout);

	lookupDeep = (unsigned short*) malloc(SCREEN_H*SCREEN_W*sizeof(unsigned short));
	lookupFlat = (unsigned short*) malloc(SCREEN_H*SCREEN_W*sizeof(unsigned short));
	lookupDeep2 = (unsigned short*) malloc(SCREEN_H*SCREEN_W*sizeof(unsigned short));
	lookupFlat2 = (unsigned short*) malloc(SCREEN_H*SCREEN_W*sizeof(unsigned short));

    CalcTunnelLookup(lookupDeep, 1, 1);
    printf(".");
    fflush(stdout);

    CalcTunnelLookup(lookupDeep2, 0, 1);
    printf(".");
    fflush(stdout);

    CalcTunnelLookup(lookupFlat, 1, 0);
    printf(".");
    fflush(stdout);

    CalcTunnelLookup(lookupFlat2, 0, 0);
    printf(".");
    fflush(stdout);

    SetTunnelMap(data[BmapTunnel].dat, data[Bback1].dat);
    
    K3DInitObject(&cube);
    K3DInitObject(&patate);
    K3DInitObject(&aim);
    K3DInitObject(&city);
    K3DInitObject(&bizar);
    K3DInitObject(&bizar2);
    K3DLoadObjectASC("world.asc", &city);
    K3DLoadObjectASC("cube.asc", &cube);
    K3DLoadObjectASC("patate.asc", &patate);
    K3DLoadObjectASC("bizar2.asc", &bizar);
    K3DLoadObjectASC("bizar.asc", &bizar2);
    InitAim(&aim);
}

RLE_SPRITE *stretch_rle(RLE_SPRITE *rle)
{
    BITMAP *tmp;
    BITMAP *tmp2;
	RLE_SPRITE *dest;
    int i;

    tmp = create_bitmap(320,200);
    tmp2 = create_bitmap(SCREEN_W, SCREEN_H);
    clear(tmp);
    draw_rle_sprite(tmp, rle, 0, 0);
    stretch_blit(tmp, tmp2, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
	dest = get_rle_sprite(tmp2);
	destroy_bitmap(tmp);
	destroy_bitmap(tmp2);
	return dest;
}

// Draws some blobs onto an image
void PartParticles(void)
{
#define NumBlobs 255
#define IncBlob 2

    BITMAP *image,*lum,*plasma;
    BITMAP* ball;
    int i;
    TDeformTable deform;
    int position;

    ball = create_bitmap(16, 16);
    lum = create_bitmap(SCREEN_W, SCREEN_H);
    plasma = create_bitmap(320, 200);

    image = create_bitmap(SCREEN_W, SCREEN_H);; //data[Bback1].dat;
    set_palette(data[Pstyler].dat);

    clear(ball);

    for (i = 0;i < 8;i++)
    {
        int c;

        c = 256-8*((i*i)/4);
        c = 256-i*32;
        if (c > 255)
            c = 255;
        if (c < 0)
            c = 0;
        circle(ball, 8, 7, i, c);
        circle(ball, 7, 7, i, c);
        circle(ball, 7, 8, i, c);
        circle(ball, 8, 8, i, c);
    }


    deform.param[0].freq = 0x20000;
    deform.param[1].freq = 0x90000;
    deform.param[2].freq = 0x50000;
    deform.param[3].freq = 0x10000;


    while (GetPosition() < 0x0b00)
    {
        color_map = &tableGlenz1;

        clear_to_color(lum, 127+fixtoi(127*fsin(VBLframe<<16)));

		position = GetPosition();
        if (position == 0x0900 ||
        	position == 0x0906 ||
        	position == 0x0910 ||
        	position == 0x0916 ||
        	position == 0x0920 ||
        	position == 0x0926 ||
        	position == 0x0930 ||
        	position == 0x0936 ) {
		        clear_to_color(lum, 255);
        	}

        color_map = &tableBalls;

        DoDeformTable(&deform, fixtoi(63*fcos(itofix(VBLframe))));

        deform.param[0].phase = 7*VBLframe;//fixtoi(256*fcos(itofix(VBLframe)));
        deform.param[1].phase =-3*VBLframe;
        deform.param[2].phase = VBLframe/5;
        deform.param[3].phase =-VBLframe;

        for (i = 0;i <= NumBlobs;i += IncBlob)
            draw_trans_sprite(lum, ball,
                              fixtoi(itofix(SCREEN_W/2-8)+SCREEN_W/2*deform.table[i]*fcos(itofix(i))/256),
                              fixtoi(itofix(SCREEN_H/2-8)+SCREEN_H/2*deform.table[i]*fsin(itofix(i))/256)
                             );

        for (i = 0;i <= NumBlobs;i += IncBlob)
            draw_trans_sprite(lum, ball,
                              fixtoi(itofix(SCREEN_W/2-8)+SCREEN_W/4*deform.table[(-i)&255]*fcos(itofix(i))/256),
                              fixtoi(itofix(SCREEN_H/2-8)+SCREEN_H/4*deform.table[(-i)&255]*fsin(itofix(i))/256)
                             );

        if (GetPosition() < 0x0900)
            DrawPlasma(plasma, data[Bplasma].dat, VBLframe, 0, 0);
        else
            if (GetPosition() < 0x0A00)
                DrawPlasma(plasma, data[Bplasma].dat, 2*VBLframe, 255, 0);
            else
                DrawPlasma(plasma, data[Bplasma].dat, 2*VBLframe, 255, 1);

		stretch_blit(plasma, image, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
        color_map = &light1;

        {
            int n = SCREEN_W*SCREEN_H;
            unsigned char *S,*D,*d,*b;
            S=(unsigned char *)image->dat;
            D=(unsigned char *)buffer->dat;
            d=(unsigned char *)lum->dat;
            b=(unsigned char *)color_map->data;

            while (--n>=0) {
                *D++ = b[(*d++) + (*S++)*256];
            }
        }

        color_map = &tableGlenz1;

        DrawCounter();
        Update();

        while (key[KEY_F1])
        {
        }

        if (key[KEY_ESC])
            return;
    }

}


// Cubes in motion
void PartGlenzCube(void)
{
    BITMAP *image,*deform;
    K3DTObject glenz,cube;
    BITMAP * map;

    map = data[Bmap128].dat;
    K3DInit();
    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);

    K3DInitObject(&cube);
    K3DLoadObjectASC("cube.asc", &cube);
    K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
    K3DPlaceObject(&cube, 0, 0, itofix(100), 0, 0, 0);
    K3DSetObjectMap(&cube, map);

    K3DRotateObject(&cube);
    K3DProjectObject(&cube);
    K3DEnvMapObject(&cube);

    K3DInitObject(&glenz);
    K3DLoadObjectASC("cube.asc", &glenz);
    K3DSetObjectRendering(&glenz, FLAT, NONE, Z);
    K3DPlaceObject(&glenz, 0, 0, itofix(100), 0, 0, 0);
    K3DSetObjectMap(&glenz, map);

    K3DRotateObject(&glenz);
    K3DProjectObject(&glenz);
    K3DEnvMapObject(&glenz);

    image = data[Bdeform].dat;
    deform = create_bitmap(640, 480);
    blit(data[Bdeform].dat, deform, 0, 0, 0, 0, 640, 400);
    set_palette(data[Pstyler].dat);

    CreateDeformBitmap(deform);
    color_map = &tableGlenz3;

    for (;;)
    {
        WaitVBL();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        DeformBitmapScroll(buffer, image, deform,
                           160+fixtoi(160*fcos(VBLframe<<16)), 100+fixtoi(100*fsin(VBLframe<<16)),
                           160+fixtoi(160*fsin(VBLframe<<16)), 100+fixtoi(100*fcos(VBLframe<<16))
                          );

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(300),
                       VBLframe<<14, VBLframe<<15, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        K3DPlaceObject(&glenz, itofix(0), itofix(0), itofix(150),
                       VBLframe<<14, VBLframe<<15, VBLframe<<14);
        K3DRenderObject(buffer, &glenz);

        K3DPlaceObject(&glenz, itofix(0), itofix(0), itofix(225)+60*(fsin(itofix(3*VBLframe&255))),
                       VBLframe<<14, VBLframe<<15, VBLframe<<14);
        K3DRenderObject(buffer, &glenz);

        while (key[KEY_F1])
        {
        }

        if (key[KEY_ESC])
            return;
    }

    K3DDeleteObject(&glenz);
    K3DDeleteObject(&cube);
}


void PartTitle(void)
{
    PALETTE black,pal;
    BITMAP *tmp;
    BITMAP *tmp2;
	RLE_SPRITE *philo1,*philo2,*philo3,*title;
    int i;

    tmp = create_bitmap(320,200);
    tmp2 = create_bitmap(SCREEN_W, SCREEN_H);
    
    draw_rle_sprite(tmp, data[Bphilo1].dat, 0, 0);
    stretch_blit(tmp, tmp2, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
	philo1 = get_rle_sprite(tmp2);
    clear(tmp);
    draw_rle_sprite(tmp, data[Bphilo2].dat, 0, 0);
    stretch_blit(tmp, tmp2, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
	philo2 = get_rle_sprite(tmp2);
    clear(tmp);
    draw_rle_sprite(tmp, data[Bphilo3].dat, 0, 0);
    stretch_blit(tmp, tmp2, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
	philo3 = get_rle_sprite(tmp2);
    clear(tmp);
    draw_rle_sprite(tmp, data[Btitle].dat, 0, 0);
    stretch_blit(tmp, tmp2, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
	title = get_rle_sprite(tmp2);

    for (i = 0;i < 256;i++)
    {
        black[i].r = black[i].g = black[i].b = 0;
    }
    set_palette(black);

    while (GetPosition() < 0x0800)
    {
        WaitVBL();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        color_map = &light1;
        drawRotatedBack(buffer, data[Bmap256c].dat, VBLframe<<16);
        color_map = &tableGlenz1;

        if ((GetPosition() > 0x0420) && (GetPosition() < 0x0460))
        {
            fade_interpolate(black, data[Pstyler].dat, pal, GetPosition()-0x0420 , 0, 255);
            set_palette(pal);
        }
        if (GetPosition() > 0x0460)
        {
            set_palette(data[Pstyler].dat);
        }

        if ((GetPosition() >= 0x0500) && (GetPosition() < 0x0520))
            draw_trans_rle_sprite(buffer, philo1, 0, 0);

        if ((GetPosition() >= 0x0520) && (GetPosition() < 0x0600))
            draw_trans_rle_sprite(buffer, philo2, 0, 0);

        if ((GetPosition() >= 0x0600) && (GetPosition() < 0x0700))
            draw_trans_rle_sprite(buffer, philo3, 0, 0);

        if ((GetPosition() >= 0x0700))
            draw_trans_rle_sprite(buffer, title, 0, 0);

        if (key[KEY_ESC])
            return;
    }
}


void PartCredits()
{
    RLE_SPRITE *name;
    RLE_SPRITE *work;
    //RLE_SPRITE *img;
    PALETTE black,pal;
    int i;
    int people = 1,pause = 1;

    //img = data[Blogo].dat;
    set_palette(data[Plogo].dat);

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    K3DSetObjectRendering(&aim, FLAT, QSORT, Z);
    K3DPlaceObject(&aim, 0, 0, itofix(100), 0, 0, 0);

    K3DSetObjectRendering(&cube, FLAT, QSORT, Z);


    for (i = 0;i < 256;i++)
    {
        black[i].r = black[i].g = black[i].b = 0;
    }
    set_palette(black);

    stretch_blit(data[Blogo].dat, buffer, 0, 40, 640, 400, 0, 0, SCREEN_W, SCREEN_H);
	Update();
 
    for (i = 0;i < 63;i++)
    {
        fade_interpolate(black, data[Plogo].dat, pal, i, 0, 255);
        vsync();
		Update();
        set_palette(pal);
    }
    name = data[Bimpulse].dat;
    work = data[Bcode].dat;

    while (GetPosition() < 0x0200)
    {
  		Update();
    }

    while (GetPosition() < 0x0330)
    {
        stretch_blit(data[Blogo].dat, buffer, 0, 40, 640, 400, 0, 0, SCREEN_W, SCREEN_H);

        if ((GetPosition()&255) < 5)
        {
            if (!pause)
            {
                people++;
                pause = 1;
            }
        }
        else
        {
            pause = 0;
        }
        switch (people)
        {
        case 1:
            name = data[Bimpulse].dat;
            work = data[Bcode].dat;
            break;
        case 2:
            name = data[Bbul].dat;
            work = data[Bgfx].dat;
            break;
        case 3:
            name = data[Bvicenzo].dat;
            work = data[Bgfx].dat;
            break;
        case 4:
            name = data[BlogicDream].dat;
            work = data[Bmusic].dat;
            break;
        }


        draw_rle_sprite(buffer, work, 0, 0);
        draw_rle_sprite(buffer, name, SCREEN_W/2-160, SCREEN_H/2-100);

        K3DPlaceObject(&aim, 0, 0, itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+0)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, 0, 0, itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+64)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, 0, 0, itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+128)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, 0, 0, itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+192)<<16);
        K3DRenderObject(buffer, &aim);


        if (key[KEY_ESC])
            return;

		Update();
    }

    stretch_blit(data[Blogo].dat, buffer, 0, 40, 640, 400, 0, 0, SCREEN_W, SCREEN_H);
    Update();
    for (i = 63;i > 0;i--)
    {
        fade_interpolate(black, data[Plogo].dat, pal, i, 0, 255);
        vsync();
        set_palette(pal);
    }

    while (GetPosition() < 0x0420)
    {
	    Update();
    }
}

void PartTunnel(void)
{
    TDeformTable deform;

	RLE_SPRITE *greets1, *greets2;
	greets1 = stretch_rle(data[Bgreets1].dat);
	greets2 = stretch_rle(data[Bgreets2].dat);

    set_palette(data[Pstyler].dat);
    deform.param[0].freq = 0x10000;
    deform.param[1].freq = 0x10000;
    deform.param[2].freq = 0x10000;
    deform.param[3].freq = 0x10000;

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    K3DSetObjectRendering(&aim, FLAT, NONE, Z);

    color_map = &tableGlenz1;

    while (GetPosition() < 0x0d00)
    {
        color_map = &tableGlenz1;
        DoDeformTable(&deform, 0*VBLframe);

        if (GetPosition() > 0x0c00)
        {
            deform.param[0].freq = 0x10000;
            deform.param[1].freq = 0x20000;
            deform.param[2].freq = 0x30000;
            deform.param[3].freq = 0x10000;
            deform.param[0].phase = VBLframe/2;
            deform.param[1].phase = 5*VBLframe;
            deform.param[2].phase = 2*VBLframe;
            deform.param[3].phase = VBLframe/4;
        }

        DrawTunnel(buffer, lookupDeep2, VBLframe, 3*VBLframe, &deform);
        color_map = &tableGlenz1;
        if (GetPosition() > 0x0c00)
        {
            if (GetPosition()&8)
                draw_trans_rle_sprite(buffer, greets1, 0, 0);
            else
                draw_trans_rle_sprite(buffer, greets2, 0, 0);
        }
        DrawCounter();
        Update();
    }

    drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);

    while (GetPosition() < 0x0e00)
    {
        color_map = &tableGlenz3;
        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        DoDeformTable(&deform, VBLframe);

        deform.param[0].freq = 0x10000;
        deform.param[1].freq = 0x30000;
        deform.param[2].freq = 0x20000;
        deform.param[3].freq = 0x10000;
        deform.param[0].phase = VBLframe/3;
        deform.param[1].phase = 1*VBLframe;
        deform.param[2].phase = 2*VBLframe;
        deform.param[3].phase = VBLframe/4;

        DrawTunnel(buffer, lookupDeep, 3*VBLframe, VBLframe, &deform);

        color_map = &tableGlenz3;
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(VBLframe)), 0, 0, (VBLframe+0)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(VBLframe)), 0, 0, (VBLframe+64)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(VBLframe)), 0, 0, (VBLframe+128)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(VBLframe)), 0, 0, (VBLframe+192)<<16);
        K3DRenderObject(buffer, &aim);

        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(2*VBLframe+128)), 0, 0, (VBLframe+32)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(2*VBLframe+128)), 0, 0, (VBLframe+64+32)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(2*VBLframe+128)), 0, 0, (VBLframe+128+32)<<16);
        K3DRenderObject(buffer, &aim);
        K3DPlaceObject(&aim, itofix(0), itofix(0), itofix(200)+50*fsin(itofix(2*VBLframe+128)), 0, 0, (VBLframe+192+32)<<16);
        K3DRenderObject(buffer, &aim);

        DrawCounter();
		Update();
    }
}


void PartVertexPlasma(void)
{
    int i,y,x;
    BITMAP *img,*map,*mask;
    TDeformTable deform;
    TDeformTable plasma;
    int correct = 0;

    img = data[Bmap128].dat;
    for (x = 0;x < 128;x++)
        for (y = 0;y < 128;y++)
            _putpixel(img, x, y, _getpixel(img, x, y)+128);


    mask = data[Bseuil128].dat;
    map = create_bitmap(128, 128);
    blit(img, map, 0, 0, 0, 0, 128, 128);


    set_palette(data[Pmask].dat);

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    K3DSetObjectRendering(&patate, TEXTURE, QSORT, Z);
    K3DSetObjectMap(&patate, map);
    K3DPlaceObject(&patate, 0, 0, itofix(400), 0, 0, 0);
    K3DRotateObject(&patate);
    K3DProjectObject(&patate);
    K3DEnvMapObject(&patate);

    K3DSetObjectRendering(&cube, TEXTURE_MASK_CORRECTED, QSORT, Z);
    K3DSetObjectMap(&cube, map);
    K3DPlaceObject(&cube, 0, 0, itofix(400), 0, 0, 0);
    K3DRotateObject(&cube);
    K3DProjectObject(&cube);
    K3DEnvMapObject(&cube);

    deform.param[0].freq = 0x10000;
    deform.param[1].freq = 0x20000;
    deform.param[2].freq = 0x20000;
    deform.param[3].freq = 0x10000;

    plasma.param[0].freq = 0x10000;
    plasma.param[1].freq = 0x20000;
    plasma.param[2].freq = 0x30000;
    plasma.param[3].freq = 0x50000;

    color_map = &light1;

    while (GetPosition() < 0x1000)
    {
        color_map = &tableGlenz1;

        DoDeformTable(&deform, 0*VBLframe);
        deform.param[0].phase = 1*VBLframe;
        deform.param[1].phase = 7*VBLframe;
        deform.param[2].phase = 3*VBLframe;
        deform.param[3].phase = 4*VBLframe;
        DrawTunnel(buffer, lookupFlat2, VBLframe, VBLframe, &deform);

        K3DPlaceObject(&patate, 80*fcos(itofix(VBLframe/2)), itofix(0), itofix(250), (VBLframe)<<14, (VBLframe)<<15, (VBLframe)<<14);
        K3DRotateObject(&patate);

        DoDeformTable(&plasma, VBLframe);
        plasma.param[0].phase = 1*VBLframe;
        plasma.param[1].phase = 9*VBLframe;
        plasma.param[2].phase = 2*VBLframe;
        plasma.param[3].phase = 3*VBLframe;

        for (i = 0;i < patate.vertexes;i++)
        {
            patate.rotated[i].x += plasma.table[(i)&255]<<14;
            patate.rotated[i].y += plasma.table[(i)&255]<<14;
        }


        if (GetPosition() == 0x0f00)
        {
            correct = VBLframe;
            K3DSetObjectRendering(&patate, TEXTURE_MASK, QSORT, Z);
        }

        K3DProjectObject(&patate);
        if (GetPosition() > 0x0f00)
        {
            int seuil;

            seuil = (VBLframe-correct)/2;
            for (i = 128*128-1;i >= 0;i--)
            {
                if (*((unsigned char*)((BITMAP*)mask)->dat+i) > seuil)
                    *((unsigned char*)((BITMAP*)map)->dat+i) = *((unsigned char*)((BITMAP*)img)->dat+i);
                else
                    *((unsigned char*)((BITMAP*)map)->dat+i) = 0;
            }
        }
        else
            K3DBackFaceObject(&patate);

        K3DDrawObject(buffer, &patate);
        
        DrawCounter();
        Update();
    }

    correct = VBLframe;
    // now the cube...
    while (GetPosition() < 0x1100)
    {
        int seuil;

        DoDeformTable(&deform, 0*VBLframe);
        deform.param[0].phase = 1*VBLframe;
        deform.param[1].phase = 7*VBLframe;
        deform.param[2].phase = 3*VBLframe;
        deform.param[3].phase = 4*VBLframe;
        DrawTunnel(buffer, lookupFlat, VBLframe, VBLframe, &deform);

        K3DPlaceObject(&cube, 80*fcos(itofix(VBLframe/2)), itofix(0), itofix(150), (VBLframe)<<14, (VBLframe)<<15, (VBLframe)<<14);
        K3DRotateObject(&cube);

        K3DProjectObject(&cube);
        seuil = 128-(VBLframe-correct)/2;
        for (i = 128*128-1;i >= 0;i--)
        {
            if (*((unsigned char*)((BITMAP*)mask)->dat+i) > seuil)
                *((unsigned char*)((BITMAP*)map)->dat+i) = *((unsigned char*)((BITMAP*)img)->dat+i);
            else
                *((unsigned char*)((BITMAP*)map)->dat+i) = 0;
        }
        K3DDrawObject(buffer, &cube);
        
        DrawCounter();
        Update();
    }

}

void PartPulseCube(void)
{
    BITMAP *img,*deform,*map;
    int i;

    img = data[Bdeform].dat;
    deform = data[Bdeform].dat;

    set_palette(data[Pstyler].dat);

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);

    K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
    K3DSetObjectMap(&cube, data[Bmap256c].dat);
    K3DPlaceObject(&cube, 0, 0, itofix(400), 0, 0, 0);
    K3DRotateObject(&cube);
    K3DProjectObject(&cube);
    K3DEnvMapObject(&cube);

    K3DSetObjectRendering(&patate, TEXTURE_SHADED, QSORT, Z);
    K3DSetObjectMap(&patate, data[Bmap256c].dat);
    K3DPlaceObject(&patate, 0, 0, itofix(400), 0, 0, 0);
    K3DRotateObject(&patate);
    K3DProjectObject(&patate);
    K3DEnvMapObject(&patate);

    color_map = &tableGlenz3;
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

    while (GetPosition() < 0x1300)
    {
        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz1;

        DeformBitmapScroll(buffer, img, deform, 150+fixtoi(150*fsin(itofix(VBLframe)))
                           , 90+fixtoi(90*fcos(itofix(3*VBLframe)))
                           , 150+fixtoi(150*fcos(itofix(3*VBLframe)))
                           , 90+fixtoi(90*fsin(itofix(2*VBLframe))));

        if (GetPosition() >= 0x1200)
        {
            set_projection_viewport(fixtoi(50*fcos(itofix(2*VBLframe))),
                                    fixtoi(50*fcos(itofix(1*VBLframe))), SCREEN_W, SCREEN_H);

            K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
            K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(300),
                           VBLframe<<16, VBLframe<<15, VBLframe<<14);
            K3DRenderObject(buffer, &cube);

            drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
            color_map = &tableGlenz3;
            K3DSetObjectRendering(&cube, FLAT, NONE, Z);
            K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(150),
                           VBLframe<<16, VBLframe<<15, VBLframe<<14);
            K3DRenderObject(buffer, &cube);

            K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(225)+60*(fsin(itofix(5*VBLframe))),
                           VBLframe<<16, VBLframe<<15, VBLframe<<14);
            K3DRenderObject(buffer, &cube);
        }
        //DrawCounter();        
        Update();
    }

    map = create_bitmap(256, 256);
    blit(data[Bmap256c].dat, map, 0, 0, 0, 0, 256, 256);
    K3DSetObjectMap(&patate, map);
    K3DSetObjectMap(&cube, map);

    while (GetPosition() < 0x1500)
    {
        color_map = &tableGlenz1;

        DeformBitmapScroll(buffer, img, deform, 150+fixtoi(150*fsin(itofix(VBLframe)))
                           , 90+fixtoi(90*fcos(itofix(3*VBLframe)))
                           , 150+fixtoi(150*fcos(itofix(3*VBLframe)))
                           , 90+fixtoi(90*fsin(itofix(2*VBLframe))));

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

        // first cube
        set_projection_viewport(0, 0, SCREEN_W/2, SCREEN_H/2);

        K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(300),
                       VBLframe<<16, VBLframe<<15, VBLframe<<16);
        K3DRenderObject(buffer, &cube);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;
        K3DSetObjectRendering(&cube, FLAT, NONE, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(150),
                       VBLframe<<16, VBLframe<<15, VBLframe<<16);
        K3DRenderObject(buffer, &cube);

        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(225)+60*(fsin(itofix(5*VBLframe))),
                       VBLframe<<16, VBLframe<<15, VBLframe<<16);
        K3DRenderObject(buffer, &cube);

        // 2e cube
        set_projection_viewport(SCREEN_W-SCREEN_W/2, 0, SCREEN_W/2, SCREEN_H/2);

        K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(300),
                       VBLframe<<16, VBLframe<<16, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;
        K3DSetObjectRendering(&cube, FLAT, NONE, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(150),
                       VBLframe<<16, VBLframe<<16, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(225)+60*(fsin(itofix(5*VBLframe))),
                       VBLframe<<16, VBLframe<<16, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        // 3e cube
        set_projection_viewport(0, SCREEN_H-SCREEN_H/2, SCREEN_W/2, SCREEN_H/2);

        K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(300),
                       VBLframe<<15, VBLframe<<15, VBLframe<<16);
        K3DRenderObject(buffer, &cube);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;
        K3DSetObjectRendering(&cube, FLAT, NONE, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(150),
                       VBLframe<<15, VBLframe<<15, VBLframe<<16);
        K3DRenderObject(buffer, &cube);

        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(225)+60*(fsin(itofix(5*VBLframe))),
                       VBLframe<<15, VBLframe<<15, VBLframe<<16);
        K3DRenderObject(buffer, &cube);

        // 4e cube
        set_projection_viewport(SCREEN_W-SCREEN_W/2, SCREEN_H-SCREEN_H/2, SCREEN_W/2, SCREEN_H/2);

        K3DSetObjectRendering(&cube, TEXTURE, QSORT, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(300),
                       VBLframe<<14, VBLframe<<16, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;
        K3DSetObjectRendering(&cube, FLAT, NONE, Z);
        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(150),
                       VBLframe<<14, VBLframe<<16, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        K3DPlaceObject(&cube, itofix(0), itofix(0), itofix(225)+60*(fsin(itofix(5*VBLframe))),
                       VBLframe<<14, VBLframe<<16, VBLframe<<14);
        K3DRenderObject(buffer, &cube);

        set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
        K3DPlaceObject(&patate, itofix(0), itofix(0), itofix(300)+60*(fsin(itofix(5*VBLframe))),
                       VBLframe<<15, VBLframe<<16, (-VBLframe)<<14);

        color_map = &light2;
        K3DRotateObject(&patate);
        K3DProjectObject(&patate);
        K3DBackFaceObject(&patate);
        for (i = 0;i < patate.vertexes;i++)
        {
            patate.projected[i].color = 110+fixtoi(40*fsin(itofix(9*VBLframe)))+
                                        fixtoi(40*fcos(itofix(4*(VBLframe+i))));
        }
        K3DDrawObject(buffer, &patate);

        DrawCounter();
		Update();
    }
}

void PartCity(void)
{
    RLE_SPRITE *mask, *world;

    mask = stretch_rle(data[Bmask1].dat);
    world = stretch_rle(data[Bworld].dat);

    K3DSetObjectRendering(&city, FLAT, NONE, Z);
    K3DSetObjectMap(&city, data[Bmap256c].dat);
    K3DPlaceObject(&city, 0, 0, itofix(400), 0, 0, 0);
    K3DRotateObject(&city);
    K3DProjectObject(&city);
    K3DEnvMapObject(&city);

    //  set_palette(data[Pstyler].dat);

    city.zmax = 64;


    while (GetPosition() < 0x1700)
    {
        color_map = &tableGlenz1;
        stretch_blit(data[Bdeform].dat, buffer,
                     SCREEN_W/2+fixtoi(150*fcos(itofix(VBLframe))),
                     SCREEN_H/2+fixtoi(90*fsin(itofix(2*VBLframe))),
                     SCREEN_W/2+fixtoi(80*fsin(itofix(3*VBLframe))),
                     SCREEN_H/2+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, SCREEN_W, SCREEN_H);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&city, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<16), itofix(40)+20*fcos(VBLframe<<14), 128<<16, VBLframe<<16);
        K3DRenderObject(buffer, &city);

        color_map = &tableGlenz1;
        draw_trans_rle_sprite(buffer, mask, 0, 0);

        switch (GetPosition()&3)
        {
        case 0:
            color_map = &tableGlenz1;
            draw_trans_rle_sprite(buffer, world, 0, 0);
            break;
        case 1:
            color_map = &tableGlenz2;
            draw_trans_rle_sprite(buffer, world, 0, 0);
            break;
        case 2:
            color_map = &tableGlenz3;
            draw_trans_rle_sprite(buffer, world, 0, 0);
            break;
        case 3:

            break;
        }

        DrawCounter();
        Update();

        if (key[KEY_ESC])
            break;
    }
}


void PartEye(void)
{
    RLE_SPRITE *head, *knights, *kills, *eyes, *eye;

    head = stretch_rle(data[Bhead].dat);
    knights = stretch_rle(data[Bknights].dat);
    kills = stretch_rle(data[Bkills].dat);
    eyes = stretch_rle(data[Beyes].dat);
    eye = stretch_rle(data[Beye].dat);

    K3DSetObjectRendering(&bizar, FLAT, NONE, Z);
    K3DSetObjectRendering(&bizar2, FLAT, NONE, Z);
    set_palette(data[Phead].dat);

    bizar.zmax = 64;

    while (GetPosition() < 0x1800)
    {
        color_map = &tableGlenz1;
        stretch_blit(data[Bdeform].dat, buffer,
                     SCREEN_W/2+fixtoi(150*fcos(itofix(VBLframe))),
                     SCREEN_H/2+fixtoi(90*fsin(itofix(2*VBLframe))),
                     SCREEN_W/2+fixtoi(80*fsin(itofix(3*VBLframe))),
                     SCREEN_H/2+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, SCREEN_W, SCREEN_H);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 0<<16);
        K3DPlaceObject(&bizar2, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 0<<16);
        if (GetPosition()&4)
            K3DRenderObject(buffer, &bizar);
        else
            K3DRenderObject(buffer, &bizar2);

        if (GetPosition()&4)
            draw_rle_sprite(buffer, head, 0, 0);

        if (GetPosition()&8)
            set_palette(data[Phead].dat);
        else
            set_palette(head_pal);

        if (key[KEY_ESC])
            break;
            
        DrawCounter();
        Update();
    }

    while (GetPosition() < 0x1900)
    {
        color_map = &tableGlenz1;

        stretch_blit(data[Bdeform].dat, buffer,
                     SCREEN_W/2+fixtoi(150*fcos(itofix(VBLframe))),
                     SCREEN_H/2+fixtoi(90*fsin(itofix(2*VBLframe))),
                     SCREEN_W/2+fixtoi(80*fsin(itofix(3*VBLframe))),
                     SCREEN_H/2+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, SCREEN_W, SCREEN_H);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar2, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 0<<16);
        K3DRenderObject(buffer, &bizar2);

        color_map = &tableGlenz1;

        if (GetPosition() < 0x1810)
            draw_trans_rle_sprite(buffer, knights, 0, 0);
        if ((GetPosition() >= 0x1810) && (GetPosition() < 0x1820))
            draw_trans_rle_sprite(buffer, kills, 0, 0);
        if ((GetPosition() >= 0x1820) && (GetPosition() < 0x1830))
            draw_trans_rle_sprite(buffer, eyes, 0, 0);
        if (GetPosition() > 0x1830)
            draw_trans_rle_sprite(buffer, eye, 0, 0);

        if (key[KEY_ESC])
            break;
            
        DrawCounter();
        Update();
    }
}

void PartEyeGore(void)
{
    BITMAP * bufferEye;
    int correct;

    clear(screen);
    bufferEye = create_bitmap(358, 363);
    set_palette(data[Pend].dat);
    blit(data[Bend].dat, screen, 0, 0, (SCREEN_W-358)/2, (SCREEN_H-363)/2, 358, 363);

    while (GetPosition() <= 0x1a00)
    {
        WaitVBL();

        if (key[KEY_ESC])
            break;
    }

    color_map = &tableEye;
    correct = VBLframe;
    while (GetPosition() < 0x1b00)
    {
        WaitVBL();
        blit(bufferEye, screen, 0, 0, (SCREEN_W-358)/2, (SCREEN_H-363)/2, 358, 363);
        ThresholdSmooth(bufferEye, data[Bend].dat, data[Bseuil].dat, 256-(VBLframe-correct));
        if (key[KEY_ESC])
            break;
    }
}

void PartFinal(void)
{
    int fade = 63;
    PALETTE black,pal;
    int i;

    //  CalcBump(data[Bdeform].dat);
	RLE_SPRITE *knights, *title;

	knights = stretch_rle(data[Bknights].dat);
	title = stretch_rle(data[Btitle].dat);

    for (i = 0;i < 256;i++)
    {
        black[i].r = black[i].g = black[i].b = 0;
    }


    K3DSetObjectRendering(&bizar, FLAT, NONE, Z);
    K3DSetObjectRendering(&bizar2, FLAT, NONE, Z);
    set_palette(data[Pstyler].dat);

    bizar.zmax = 64;

    while (GetPosition() < 0x1c00)
    {
        color_map = &tableGlenz1;

        stretch_blit(data[Bdeform].dat, buffer,
                     SCREEN_W/2+fixtoi(150*fcos(itofix(VBLframe))),
                     SCREEN_H/2+fixtoi(90*fsin(itofix(2*VBLframe))),
                     SCREEN_W/2+fixtoi(80*fsin(itofix(3*VBLframe))),
                     SCREEN_H/2+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, SCREEN_W, SCREEN_H);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 30*fsin(VBLframe<<16));
        K3DRenderObject(buffer, &bizar);

        if (key[KEY_ESC])
            break;
            
        DrawCounter();
        Update();
    }

    while (GetPosition() <= 0x2000)
    {
        color_map = &tableGlenz1;
        stretch_blit(data[Bdeform].dat, buffer,
                     SCREEN_W/2+fixtoi(150*fcos(itofix(VBLframe))),
                     SCREEN_H/2+fixtoi(90*fsin(itofix(2*VBLframe))),
                     SCREEN_W/2+fixtoi(80*fsin(itofix(3*VBLframe))),
                     SCREEN_H/2+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, SCREEN_W, SCREEN_H);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar2, 160*fsin(VBLframe<<15), itofix(100), itofix(200)+100*fsin(VBLframe<<14), 50*fsin(VBLframe<<16), VBLframe<<15, 30*fsin(VBLframe<<16));
        K3DRenderObject(buffer, &bizar2);

        if (GetPosition() <= 0x1e00)
        {
            K3DPlaceObject(&bizar, 100*fcos(VBLframe<<14), itofix(100), itofix(300)+200*fsin(VBLframe<<16), 50*fsin(VBLframe<<15), VBLframe<<14, 40*fsin(VBLframe<<15));
            K3DRenderObject(buffer, &bizar);
        }

        color_map = &tableGlenz1;
        if ((GetPosition() > 0x1e00) && (GetPosition() & 4))
            draw_trans_rle_sprite(buffer, knights, 0, 0);

        if (GetPosition() > 0x1e00)
        {
            switch (GetPosition()&15)
            {
            case 0:
                color_map = &tableGlenz1;
                draw_trans_rle_sprite(buffer, knights, 0, 0);
                break;
            case 3:
                color_map = &tableGlenz2;
                draw_trans_rle_sprite(buffer, knights, 0, 0);
                break;
            case 5:
                color_map = &tableGlenz3;
                draw_trans_rle_sprite(buffer, knights, 0, 0);
                break;
            case 7:

                break;
            case 9:
                color_map = &tableGlenz1;
                draw_trans_rle_sprite(buffer, title, 0, 0);
                break;
            case 11:
                color_map = &tableGlenz2;
                draw_trans_rle_sprite(buffer, title, 0, 0);
                break;
            case 13:
                color_map = &tableGlenz3;
                draw_trans_rle_sprite(buffer, title, 0, 0);
                break;
            case 15:

                break;
            }
        }

        if (GetPosition() >= 0x1f00)
        {
            fade_interpolate(black, data[Pstyler].dat, pal, fade, 0, 255);
            set_palette(pal);
            if (fade >= 0) fade--;
        }

        if (key[KEY_ESC])
            break;
        DrawCounter();
        Update();
    }
}

void Demo(void)
{
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    
    PartCredits();
    PartTitle();

    PartParticles();
    PartTunnel();
    PartVertexPlasma();
    PartPulseCube();
    PartCity();
    PartEye();
    PartEyeGore();
    PartFinal();


    destroy_bitmap(buffer);
    K3DDeleteObject(&cube);
    K3DDeleteObject(&patate);
    K3DDeleteObject(&aim);
    K3DDeleteObject(&city);
    K3DDeleteObject(&bizar);
    K3DDeleteObject(&bizar2);
}





int main(int argc, char ** argv)
{
    char buf[80];
    int card, w, h;
    
    printf("Styler (c) KNIGHTS 1998-2009\n  FINAL Version\n");
    printf("  ( Run with any parameter to run sound setup )\n");
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    allegro_init();

    printf("  Loading ");
    printf(".");

    install_keyboard();
    install_mouse();

    /* load the datafile into memory */
    strcpy(buf, argv[0]);
    strcpy(get_filename(buf), "styler.dat");
    fflush(stdout);
    printf(".");
    fflush(stdout);
    data = load_datafile(buf);
    if (!data)
    {
        allegro_exit();
        printf("  Error loading styler.dat!\n\n");
        exit(1);
    }

    printf(".");
    fflush(stdout);

    printf(".");
    fflush(stdout);


    if (argc>1) 
    {
        int tokens;
        tokens = sscanf(argv[1], "%dx%d", &w, &h);
        if (tokens != 2) 
        {
            printf("  \"%s\" is not a valid display mode, try '640x480' you douche.\n", argv[1]);
        }
        set_gfx_mode(mode, w, h, 0, 0);
    }
    else 
    {
        //set_gfx_mode(mode, 320, 240, 0, 0);
        //gfx_mode_select(&card, &w, &h);
        //set_gfx_mode(card, w,h, 0,0);
        set_gfx_mode(mode, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    }

    InitDemo();

    MusicStart("styler.xm");
    SDL_AddTimer(50, MusicUpdate, 0);
    SDL_AddTimer(1000,TimerHandler, 0);
    
    Demo();

    MusicStop();

    /* unload the datafile when we are finished with it */
    unload_datafile(data);

    set_gfx_mode(GFX_TEXT, 640, 480, 0, 0);
    printf("\n       ______                                                             _____\n");
    printf("______\\\\     \\______________________________________  ___________________/    /\n");
    printf("\\      \\\\                                           |/ \"I've Got The Poison\" / \n");
    printf(" \\      |\\     .______.    ._______.   .______.     |_. .___.   .____.  .___/  \n");
    printf("  |     |/     /|     \\     |     |    / _____     _    /_  |    \\  /       \\  \n");
    printf("  |      _____/_|     _     |     |    \\_\\          |     |_|     >>\\____    \\ \n");
    printf("  |             \\\\   |      |     |\\        //      |    //    \\\\//           \\\n");
    printf("  |_____|\\_____  >>__|\\_____|_____| \\______<<  ____/|___<<  __________________/\n");
    printf(" !Fuck'Em All! \\//                          \\\\/          \\\\/      !SenseRofTHN! \n");

    printf("\nStyler [%s %s]\n\n", __DATE__, __TIME__);

//  destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN()

