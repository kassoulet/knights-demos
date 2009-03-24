/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  02/98

  (c) 1998-2002 KNIGHTS

  Main File

  gautier@tlk.fr
*/

#define USE_CONSOLE

#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "styler.h"
#include "k3d.h"
#include "effects.h"

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT= 800;

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

unsigned short lookupDeep[320*200];
unsigned short lookupFlat[320*200];
unsigned short lookupDeep2[320*200];
unsigned short lookupFlat2[320*200];

RLE_SPRITE * counter[10];
PALETTE head;

void Fatal(char* str)
{
    printf("\nFatal : %s\n", str);
    exit(0);
}



void TimerHandler(void);


void TimerHandler(void)
{
    VBLfps = VBLcount;
    VBLcount = 0;
}

void DrawCounter(void)
{
    draw_trans_rle_sprite(buffer, counter[(VBLframe/3)%9], 319-92, 0);
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

void InitDemo(void)
{
    int i;

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

    {
        PALETTE pal;

        memcpy(pal, data[Phead].dat, sizeof (pal));
        for (i = 0;i < 128;i++)
        {
            head[i].r = pal[i].r;
            head[i].g = pal[i].g;
            head[i].b = pal[i].b;
            head[i+128].r = pal[i+128].b;
            head[i+128].g = pal[i+128].r;
            head[i+128].b = pal[i+128].g;
        }
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

}


// Draws some blobs onto an image
void PartParticles(void)
{
#define NumBlobs 255
#define IncBlob 2

    BITMAP * image,*lum;
    BITMAP* ball;
    int i;
    TDeformTable deform;

    ball = create_bitmap(16, 16);
    lum = create_bitmap(320, 200);

    image = data[Bback1].dat;
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
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        clear_to_color(lum, 127+fixtoi(127*fsin(VBLframe<<16)));

        color_map = &tableBalls;

        DoDeformTable(&deform, fixtoi(63*fcos(itofix(VBLframe))));

        deform.param[0].phase = 7*VBLframe;//fixtoi(256*fcos(itofix(VBLframe)));
        deform.param[1].phase =-3*VBLframe;
        deform.param[2].phase = VBLframe/5;
        deform.param[3].phase =-VBLframe;

        for (i = 0;i <= NumBlobs;i += IncBlob)
            draw_trans_sprite(lum, ball,
                              fixtoi(itofix(160-8)+160*deform.table[i]*fcos(itofix(i))/256),
                              fixtoi(itofix(100-8)+100*deform.table[i]*fsin(itofix(i))/256)
                             );

        for (i = 0;i <= NumBlobs;i += IncBlob)
            draw_trans_sprite(lum, ball,
                              fixtoi(itofix(160-8)+80*deform.table[(-i)&255]*fcos(itofix(i))/256),
                              fixtoi(itofix(100-8)+50*deform.table[(-i)&255]*fsin(itofix(i))/256)
                             );

        if (GetPosition() < 0x0900)
            DrawPlasma(image, data[Bplasma].dat, VBLframe, 0, 0);
        else
            if (GetPosition() < 0x0A00)
                DrawPlasma(image, data[Bplasma].dat, 2*VBLframe, 255, 0);
            else
                DrawPlasma(image, data[Bplasma].dat, 2*VBLframe, 255, 1);

        color_map = &light1;


        {
            int n=320*200-1;
            unsigned char *S,*D,*d,*b;
            S=(unsigned char *)image->dat;
            D=(unsigned char *)buffer->dat;
            d=(unsigned char *)lum->dat;
            b=(unsigned char *)color_map->data;

            while (n>0) {
                *D++=b[(*d++)+(*S++)*256];
                n--;
            }


        }
        /*
         asm("
           xorl %%eax,%%eax

           mergeLum_loop:
           movb (%%edx),%%al
           incl  %%edx
           movb (%%esi),%%ah
           incl  %%esi
           movb (%%ebx,%%eax),%%al
           movb %%al,(%%edi)
           incl  %%edi

           decl %%ecx
           jnz mergeLum_loop

           "
           :
         :"S" (image->dat), "D" (buffer->dat), "d" (lum->dat), "c"(320*200-1), "b"(color_map->data)
           :"eax", "ebx", "ecx", "edx", "esi", "edi");
         */
        color_map = &tableGlenz1;
        if (GetPosition() == 0x0900)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0906)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0910)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0916)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0920)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0926)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0930)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);
        if (GetPosition() == 0x0936)
            draw_trans_rle_sprite(buffer, data[Beclair].dat, 0, 0);



        while (key[KEY_F1])
        {
        }

        if (key[KEY_ESC])
            return;
    }


    /*
    draw_trans_sprite(lum,ball,
    fixtoi(itofix(160-8)+
    80*fcos(itofix(VBLframe*i/4096))+
    80*fcos(itofix(VBLframe/4+i))),

    fixtoi(itofix(100-8)+
    50*fcos(itofix(VBLframe-i/4))+
    50*fsin(itofix(VBLframe*i/1024))));


    {
    unsigned char * dest,*src,*intensity;

    dest      = buffer->dat;
    src       = image->dat;
    intensity = lum->dat;

    for (i=320*200;i>0;i--)
    {
    *dest = color_map->data[*intensity][*src];
    dest++;
    src++;
    intensity++;
    }
    }
    */
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
//  BITMAP *buffer;
    int i;

//  buffer = create_bitmap(320,200);

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
            draw_trans_rle_sprite(buffer, data[Bphilo1].dat, 0, 0);

        if ((GetPosition() >= 0x0520) && (GetPosition() < 0x0600))
            draw_trans_rle_sprite(buffer, data[Bphilo2].dat, 0, 0);

        if ((GetPosition() >= 0x0600) && (GetPosition() < 0x0700))
            draw_trans_rle_sprite(buffer, data[Bphilo3].dat, 0, 0);

        if ((GetPosition() >= 0x0700))
            draw_trans_rle_sprite(buffer, data[Btitle].dat, 0, 0);

        if (key[KEY_ESC])
            return;
    }
}


void PartCredits()
{
    RLE_SPRITE *name;
    RLE_SPRITE *work;
    RLE_SPRITE *img;
    BITMAP *buffer640;
    PALETTE black,pal;
    int i;
    int people = 1,pause = 1;

    buffer640 = create_bitmap(640, 480);

    img = data[Blogo].dat;
    set_palette(data[Plogo].dat);

    set_projection_viewport(320, 240, 320, 240);
    set_projection_viewport(0, 0, 640, 480);
    K3DSetObjectRendering(&aim, FLAT, QSORT, Z);
    K3DPlaceObject(&aim, 0, 0, itofix(100), 0, 0, 0);

    K3DSetObjectRendering(&cube, FLAT, QSORT, Z);


    for (i = 0;i < 256;i++)
    {
        black[i].r = black[i].g = black[i].b = 0;
    }
    set_palette(black);
    draw_rle_sprite(buffer640, img, 0, 0);
    draw_rle_sprite(screen, img, 0, 0);
    for (i = 0;i < 63;i++)
    {
        fade_interpolate(black, data[Plogo].dat, pal, i, 0, 255);
        vsync();
        set_palette(pal);
    }

    name = data[Bimpulse].dat;
    work = data[Bcode].dat;

    while (GetPosition() < 0x0200)
    {
    }

    while (GetPosition() < 0x0330)
    {
        WaitVBL();

        blit(buffer640, screen, 0, 0, 0, 0, 640 , 480);
        draw_rle_sprite(buffer640, img, 0, 0);

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


        draw_rle_sprite(buffer640, work, 0, 0);
        draw_rle_sprite(buffer640, name, 285, 185);

        K3DPlaceObject(&aim, itofix(100), itofix(40), itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+0)<<16);
        K3DRenderObject(buffer640, &aim);
        K3DPlaceObject(&aim, itofix(100), itofix(40), itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+64)<<16);
        K3DRenderObject(buffer640, &aim);
        K3DPlaceObject(&aim, itofix(100), itofix(40), itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+128)<<16);
        K3DRenderObject(buffer640, &aim);
        K3DPlaceObject(&aim, itofix(100), itofix(40), itofix(240)+20*fsin(itofix(VBLframe)), 0, 0, (VBLframe+192)<<16);
        K3DRenderObject(buffer640, &aim);

        if (key[KEY_ESC])
            return;
    }

    draw_rle_sprite(screen, img, 0, 0);
    for (i = 63;i > 0;i--)
    {
        fade_interpolate(black, data[Plogo].dat, pal, i, 0, 255);
        vsync();
        set_palette(pal);
    }

    while (GetPosition() < 0x0420)
    {
    }

    destroy_bitmap(buffer640);
}

void PartTunnel(void)
{
    TDeformTable deform;


    set_palette(data[Pstyler].dat);
    deform.param[0].freq = 0x10000;
    deform.param[1].freq = 0x10000;
    deform.param[2].freq = 0x10000;
    deform.param[3].freq = 0x10000;

    set_projection_viewport(0, 0, 320, 200);
    K3DSetObjectRendering(&aim, FLAT, NONE, Z);


    color_map = &tableGlenz1;

    while (GetPosition() < 0x0d00)
    {
        WaitVBL();
        color_map = &tableGlenz1;


        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);


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
                draw_trans_rle_sprite(buffer, data[Bgreets1].dat, 0, 0);
            else
                draw_trans_rle_sprite(buffer, data[Bgreets2].dat, 0, 0);
        }
    }

    drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);

    while (GetPosition() < 0x0e00)
    {
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

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

    set_projection_viewport(0, 0, 320, 200);
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
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

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
    }

    correct = VBLframe;
    // now the cube...
    while (GetPosition() < 0x1100)
    {
        int seuil;

        WaitVBL();
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

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
    }

}

void PartPulseCube(void)
{
    BITMAP *img,*deform,*map;
    int i;

    img = data[Bdeform].dat;
    deform = data[Bdeform].dat;

    set_palette(data[Pstyler].dat);

    set_projection_viewport(0, 0, 320, 200);

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
        WaitVBL();
        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz1;
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

        DeformBitmapScroll(buffer, img, deform, 150+fixtoi(150*fsin(itofix(VBLframe)))
                           , 90+fixtoi(90*fcos(itofix(3*VBLframe)))
                           , 150+fixtoi(150*fcos(itofix(3*VBLframe)))
                           , 90+fixtoi(90*fsin(itofix(2*VBLframe))));

        if (GetPosition() >= 0x1200)
        {
            set_projection_viewport(fixtoi(50*fcos(itofix(2*VBLframe))),
                                    fixtoi(50*fcos(itofix(1*VBLframe))), 320, 200);

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
    }

    map = create_bitmap(256, 256);
    blit(data[Bmap256c].dat, map, 0, 0, 0, 0, 256, 256);
    K3DSetObjectMap(&patate, map);
    K3DSetObjectMap(&cube, map);

    while (GetPosition() < 0x1500)
    {
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

        DeformBitmapScroll(buffer, img, deform, 150+fixtoi(150*fsin(itofix(VBLframe)))
                           , 90+fixtoi(90*fcos(itofix(3*VBLframe)))
                           , 150+fixtoi(150*fcos(itofix(3*VBLframe)))
                           , 90+fixtoi(90*fsin(itofix(2*VBLframe))));

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

        // first cube
        set_projection_viewport(0, 0, 160, 100);

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
        set_projection_viewport(320-160, 0, 160, 100);

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
        set_projection_viewport(0, 200-100, 160, 100);

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
        set_projection_viewport(320-160, 200-100, 160, 100);

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

        set_projection_viewport(0, 0, 320, 200);
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


    }
}

void PartCity(void)
{
    RLE_SPRITE * mask;

    mask = data[Bmask1].dat;

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
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

        //  clear(buffer);

        stretch_blit(data[Bdeform].dat, buffer,
                     160+fixtoi(150*fcos(itofix(VBLframe))),
                     100+fixtoi(90*fsin(itofix(2*VBLframe))),
                     160+fixtoi(80*fsin(itofix(3*VBLframe))),
                     100+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, 320, 200);

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
            draw_trans_rle_sprite(buffer, data[Bworld].dat, 0, 0);
            break;
        case 1:
            color_map = &tableGlenz2;
            draw_trans_rle_sprite(buffer, data[Bworld].dat, 0, 0);
            break;
        case 2:
            color_map = &tableGlenz3;
            draw_trans_rle_sprite(buffer, data[Bworld].dat, 0, 0);
            break;
        case 3:

            break;
        }

        if (key[KEY_ESC])
            break;
    }
}


void PartEye(void)
{
    K3DSetObjectRendering(&bizar, FLAT, NONE, Z);
    K3DSetObjectRendering(&bizar2, FLAT, NONE, Z);
    set_palette(data[Phead].dat);

    bizar.zmax = 64;


    while (GetPosition() < 0x1800)
    {
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

        stretch_blit(data[Bdeform].dat, buffer,
                     160+fixtoi(150*fcos(itofix(VBLframe))),
                     100+fixtoi(90*fsin(itofix(2*VBLframe))),
                     160+fixtoi(80*fsin(itofix(3*VBLframe))),
                     100+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, 320, 200);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 0<<16);
        K3DPlaceObject(&bizar2, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 0<<16);
        if (GetPosition()&4)
            K3DRenderObject(buffer, &bizar);
        else
            K3DRenderObject(buffer, &bizar2);

        if (GetPosition()&4)
            draw_rle_sprite(buffer, data[Bhead].dat, 0, 0);

        if (GetPosition()&8)
            set_palette(data[Phead].dat);
        else
            set_palette(head);

        if (key[KEY_ESC])
            break;
    }

    while (GetPosition() < 0x1900)
    {
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

        //  clear(buffer);

        stretch_blit(data[Bdeform].dat, buffer,
                     160+fixtoi(150*fcos(itofix(VBLframe))),
                     100+fixtoi(90*fsin(itofix(2*VBLframe))),
                     160+fixtoi(80*fsin(itofix(3*VBLframe))),
                     100+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, 320, 200);

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar2, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 0<<16);
        K3DRenderObject(buffer, &bizar2);

        color_map = &tableGlenz1;

        if (GetPosition() < 0x1810)
            draw_trans_rle_sprite(buffer, data[Bknights].dat, 0, 0);
        if ((GetPosition() >= 0x1810) && (GetPosition() < 0x1820))
            draw_trans_rle_sprite(buffer, data[Bkills].dat, 0, 0);
        if ((GetPosition() >= 0x1820) && (GetPosition() < 0x1830))
            draw_trans_rle_sprite(buffer, data[Beyes].dat, 0, 0);
        if (GetPosition() > 0x1830)
            draw_trans_rle_sprite(buffer, data[Beye].dat, 0, 0);



        if (key[KEY_ESC])
            break;
    }
}

void PartEyeGore(void)
{
    BITMAP * bufferEye;
    int correct;

    bufferEye = create_bitmap(358, 363);
    set_palette(data[Pend].dat);
    blit(data[Bend].dat, screen, 0, 0, 140, 58, 358, 363);

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
        blit(bufferEye, screen, 0, 0, 140, 58, 358, 363);
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
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);


        stretch_blit(data[Bdeform].dat, buffer,
                     160+fixtoi(150*fcos(itofix(VBLframe))),
                     100+fixtoi(90*fsin(itofix(2*VBLframe))),
                     160+fixtoi(80*fsin(itofix(3*VBLframe))),
                     100+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, 320, 200);


        //    DrawBump(buffer,mouse_x,mouse_y);


        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        color_map = &tableGlenz3;

        K3DPlaceObject(&bizar, 0, itofix(100), itofix(200)+100*fsin(VBLframe<<15), 50*fsin(VBLframe<<15), VBLframe<<16, 30*fsin(VBLframe<<16));
        K3DRenderObject(buffer, &bizar);

        if (key[KEY_ESC])
            break;
    }

    while (GetPosition() <= 0x2000)
    {
        WaitVBL();
        color_map = &tableGlenz1;
        DrawCounter();
        blit(buffer, screen, 0, 0, 0, 0, 320, 200);

        //  clear(buffer);

        stretch_blit(data[Bdeform].dat, buffer,
                     160+fixtoi(150*fcos(itofix(VBLframe))),
                     100+fixtoi(90*fsin(itofix(2*VBLframe))),
                     160+fixtoi(80*fsin(itofix(3*VBLframe))),
                     100+fixtoi(50*fcos(itofix(4*VBLframe))),
                     0, 0, 320, 200);

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
            draw_trans_rle_sprite(buffer, data[Bknights].dat, 0, 0);

        if (GetPosition() > 0x1e00)
        {
            switch (GetPosition()&15)
            {
            case 0:
                color_map = &tableGlenz1;
                draw_trans_rle_sprite(buffer, data[Bknights].dat, 0, 0);
                break;
            case 3:
                color_map = &tableGlenz2;
                draw_trans_rle_sprite(buffer, data[Bknights].dat, 0, 0);
                break;
            case 5:
                color_map = &tableGlenz3;
                draw_trans_rle_sprite(buffer, data[Bknights].dat, 0, 0);
                break;
            case 7:

                break;
            case 9:
                color_map = &tableGlenz1;
                draw_trans_rle_sprite(buffer, data[Btitle].dat, 0, 0);
                break;
            case 11:
                color_map = &tableGlenz2;
                draw_trans_rle_sprite(buffer, data[Btitle].dat, 0, 0);
                break;
            case 13:
                color_map = &tableGlenz3;
                draw_trans_rle_sprite(buffer, data[Btitle].dat, 0, 0);
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
    }
}

void Demo(void)
{
//  clear(screen);
    set_gfx_mode(mode, 640, 480, 0, 0);
    clear(screen);

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

    buffer = create_bitmap(320, 200);
    PartCredits();
    set_gfx_mode(mode, 320, 200, 0, 0);

    PartTitle();

    PartParticles();
    PartTunnel();
    PartVertexPlasma();
    PartPulseCube();
    PartCity();
    PartEye();
    set_gfx_mode(mode, 640, 480, 0, 0);
    PartEyeGore();
    set_gfx_mode(mode, 320, 200, 0, 0);
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

    printf("Styler (c) KNIGHTS 1998\n  FINAL Version\n");
    printf("  ( Run with any parameter to run sound setup )\n");

    allegro_init();

    printf("  Loading ");
    printf(".");
    if (argc>1)
        mode = GFX_AUTODETECT_FULLSCREEN;

    install_timer ();
    install_keyboard ();
    reserve_voices (20, -1);

#ifdef USE_JGMOD

    if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
        printf ("Error initializing sound card");
        return 1;
    }

    set_volume(128,0);

    if (install_mod (20) < 0)
    {
        printf ("Error setting digi voices");
        return 1;
    }
#endif


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
    //  install_mouse();

    fflush(stdout);

#ifdef USE_JGMOD

    module = load_mod ("styler.xm");
    if (module == NULL)
    {
        printf ("Error reading styler.xm");
        return 1;
    }
#endif

    printf(".");
    fflush(stdout);

    InitDemo();

//  set_gfx_mode(mode, 320, 200, 0, 0);

#ifdef USE_JGMOD
    play_mod (module, TRUE);
    set_mod_volume(128);
#endif

    install_int(TimerHandler, 1000);

    Demo();
//  readkey();


#ifdef USE_JGMOD
    stop_mod ();
    destroy_mod (module);
#endif

    /* unload the datafile when we are finished with it */
    unload_datafile(data);

    set_gfx_mode(GFX_TEXT, 640, 480, 0, 0);
    printf("       ______                                                             _____\n");
    printf("______\\\\     \\______________________________________  ___________________/    /\n");
    printf("\\      \\\\                                           |/ \"I've Got The Poison\" / \n");
    printf(" \\      |\\     .______.    ._______.   .______.     |_. .___.   .____.  .___/  \n");
    printf("  |     |/     /|     \\     |     |    / _____     _    /_  |    \\  /       \\  \n");
    printf("  |      _____/_|     _     |     |    \\_\\          |     |_|     >>\\____    \\ \n");
    printf("  |             \\\\   |      |     |\\        //      |    //    \\\\//           \\\n");
    printf("  |_____|\\_____  >>__|\\_____|_____| \\______<<  ____/|___<<  __________________/\n");
    printf(" !Fuck'Em All! \\//                          \\\\/          \\\\/      !SenseRofTHN! \n");

    printf("\nStyler [%s %s]\n\n", __DATE__, __TIME__);
    printf("Contact Impulse    : Impulse.Knights@CryoGen.Com (subject: 'for impulse')\n");
    printf("                     3 place F. Buisson. 31240 St Jean. France\n");
    printf("Swapping & KNiGHTS : fgermain@NormandNet.fr & www.NormandNet.fr/~fgermain\n");
    printf("                     Knights.fr@CryoGen.Com\n");

//  destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN()

