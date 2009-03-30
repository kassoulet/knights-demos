/*

  Styler 2

  Coded by Gautier "Impulse" PORTET

  03/98  08/98

  (c) 1998-2002 KNIGHTS

  Main File

  gautier@tlk.fr
*/

#define alleg_joystick_unused 1
#define alleg_flic_unused 1
#define alleg_gui_unused 1

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "k3d.h"
#include "effects.h"
#include "styler2.h"

#include <SDL.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT= 400;

float SCALE_X;
float SCALE_Y;

int mode=GFX_AUTODETECT_WINDOWED;

extern int MusicGetPosition(void);
extern void MusicStart(char *filename);
extern void MusicStop();
extern int MusicUpdate();
extern void MusicSetPosition(int position);

extern char *_image;

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

enum UPDATE_MODE
{
    UM_WIN,
    UM_WIN2x,
    UM_FULLSCREEN,
    UM_FULLSCREEN240,
    UM_FULLSCREEN240b,
    UM_FULLSCREEN480,
    UM_FULLSCREEN480b
};

enum UPDATE_MODE UpdateMode=UM_WIN;

typedef unsigned short COLOR_MAP8[256][256];

// globals

BITMAP * buffer;
BITMAP * buffer8;

DATAFILE *data;

COLOR_MAP   tableGlenz8A;
COLOR_MAP   tableGlenz8B;
COLOR_MAP   tableGlenz8C;
COLOR_MAP   tableGlenz8D;

COLOR_MAP   tableMetaballs;


PALETTE     pal;
TDeformTable deform;


struct TBumpTable bumpTable;

COLOR_MAP8 tablePlasma;
COLOR_MAP8 table3D;

// ###############################################################
//   PART --> Styler 2 logo + Ombre <--

RLE_SPRITE *S2Ombre;
RLE_SPRITE *S2Logo;
BITMAP     *halo1,*halo2,*halo3;
BITMAP     *S2Back;
PALETTE     S2BackPal;
COLOR_MAP8  S2tableLum;
COLOR_MAP   S2tableOmbre;


// ###############################################################
//   PART --> Credits <--

#define DIVISEUR 3

typedef struct TPARTICULE
{
    int x;
    double y;
    int cr;
    int cg;
    int cb;
    double g;
}TPARTICULE;

TPARTICULE particule[40000];
int partMax=0;
int partDeb=0;

BITMAP *img_fond;
PALETTE palettefond;

BITMAP *img_code;
BITMAP *img_code2;
BITMAP *img_music;
BITMAP *img_gfx;
BITMAP *img_halob;
BITMAP *img_halor;
BITMAP *img_haloj;



// ###############################################################
//   PART --> Tunnel <--

struct K3DTObject O3DTrois;

COLOR_MAP8 tableTunnel;
unsigned short lookup1[640*400];
BITMAP *lum640;
COLOR_MAP8 table;






// ###############################################################
//   PART --> Beau-tifull picture <--

COLOR_MAP8 tableLogo;

// ###############################################################
//   PART --> Plasma <--

COLOR_MAP8 tablePlasma;
COLOR_MAP8 tablePlasmaBump;



// ###############################################################
//   PART --> Bumped Plasma <--
unsigned short lookup2[640*400];





// ###############################################################
//   PART --> Guru <--
struct K3DTObject O3DCity;
struct K3DTObject O3DSalle;
struct K3DTObject O3DGisquet;
struct K3DTObject O3DAutre;


COLOR_MAP8 tableGuru;
COLOR_MAP8 tablePlasma;


void WaitVBL();

// ###############################################################
// Synchro

void Update(BITMAP * buffer)
{
    WaitVBL();
    blit(buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
}




int VBLframe=0,VBLfps=100,VBLcount=0,VBLold=0;
int VBLdelta=0;
double fVBLdelta=0;
int refreshRate;
int finalScrollPosition;

void Fatal(char* str)
{
    set_gfx_mode(GFX_TEXT,0,0,0,0);
    printf("\nFatal : %s\n", str);
    exit(0);
}




void TimerHandler(void)
{
    VBLfps = VBLcount;
    VBLcount = 0;
}

void WaitVBL()
{
    VBLframe = SDL_GetTicks()/(1000/70);
    VBLcount++;

    VBLdelta = abs(VBLframe-VBLold);
    if (VBLfps>0)
        fVBLdelta = 50.0/VBLfps;

    VBLold = VBLframe;

    /*if (VBLdelta<1)
    {
        int now=retrace_count;
        while (retrace_count<=now)
            rest(1);
    }*/

    if (key[KEY_SPACE])
    {
        BITMAP * display;
        char buf[32];
        int i,j;


        if (key[KEY_P])
        {
            /* Screen save */
            save_bitmap("dump.tga", buffer, pal);
        }

        display = create_bitmap(8*16, 10);
        clear_to_color(display, 0);

        buf[0] = 0;
        sprintf(buf, "%3d FPS/%2d", VBLfps, VBLdelta);
        i = 8*16*2*VBLfps/70;
        if (i >= 8*16-1)
            i = 8*16-2;
        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

        for (j=1;j<=i;j++)
            vline(display, j, 0, 8, 63);

        text_mode(-1);
        textout(display, font, buf, 1, 1, makecol(250,250,250));

        blit(display, buffer, 0, 0, 0, 0, 8*16, 9);
//    blit(display, screen, 0, 0, 0, 0, 8*16, 9);

        destroy_bitmap(display);

    }
}



// ###############################################################
//

void testFeedback(void)
{
    BITMAP * feedback,*img,*mask;
    PALETTE pal;

    COLOR_MAP map;
    int i,j;
    int y=0;

    for (i = 255;i >= 0;i--)
        for (j = 255;j >= 0;j--)
        {
            int c;
            // i = lum
            // j = color

            c = j*i/255;

            if (c>256)
                c = 255;

            map.data[i][j] = c;
        }
    color_map = &map;


    feedback = create_bitmap(256,128);
    img  = load_bitmap("bumpbord.pcx",pal);
    mask = load_bitmap("mask.pcx",pal);

    set_palette(pal);
    blit(img,feedback,0,0,0,0,320,200);

    while (1)
    {
        WaitVBL();
        blit(buffer,screen,0,0,0,0,320,200);

        clear(buffer);
        set_clip(buffer,0,0,255,127);
//    drawRotatedBack2(buffer, feedback, itofix(mouse_x), mouse_y+120, VBLframe);
//    drawRotatedBack2(buffer, feedback, itofix(VBLframe), mouse_x*2, VBLframe);
        draw_sprite(buffer,mask,0,y);
        blit(buffer,feedback,0,0,0,0,256,128);
        if (key[KEY_F1])
            y++;

        if (key[KEY_F2])
            y--;

        if (key[KEY_ESC])
            break;
    }
}

// ###############################################################
//
void testFeedback2(void)
{
    BITMAP * feedback,*img,*mask,*plasma;
    PALETTE pal;

    COLOR_MAP map;
    int i,j;
    int y=0;

    for (i = 255;i >= 0;i--)
        for (j = 255;j >= 0;j--)
        {
            int c;
            // i = lum
            // j = color

            c = j+i/2;

            if (c>256)
                c = 255;

            map.data[i][j] = c;
        }
    color_map = &map;


    plasma = create_bitmap_ex(8,450,330);
    feedback = create_bitmap_ex(8,320,200);
    mask = data[BLogoOmbre].dat;
    img  = data[BBump].dat;

    clear(plasma);

    set_palette(pal);
    blit(img,feedback,0,0,0,0,320,200);

    blit(img,plasma,0,0,(450-320)/2,(330-200)/2,320,200);

    while (1)
    {
        WaitVBL();
        blit(buffer8,screen,0,0,0,0,320,200);

        DrawPlasma(buffer8, plasma, VBLframe, 64, key[KEY_F3]);

        putpixel(buffer8,100,100,255);

//    draw_sprite(buffer8,mask,(320-255)/2,y);
//    draw_trans_sprite(buffer,mask,(320-255)/2,y);


        blit(buffer8,plasma,0,0,(450-320)/2,(330-200)/2,320,200);
        for (i = 320*200;i > 0;i--)
            (*((char*)plasma->dat+i))--;

        if (key[KEY_F1])
            y++;

        if (key[KEY_F2])
            y--;

        if (key[KEY_ESC])
            break;
    }
}


// ###############################################################
//
void mergeBitmap(BITMAP *dest16,BITMAP *img8,BITMAP *lum,COLOR_MAP8 table)
{
    unsigned char  *__img,*__light;
    short *__dest;
    int j,len,h,i;

    len = dest16->w;
    if (len>img8->w)
        len = img8->w;
    if (len>lum->w)
        len = lum->w;

    h = dest16->h;
    if (h>img8->h)
        h = img8->h;
    if (h>lum->h)
        h = lum->h;

    for (j = h-1;j >= 0;j--)
    {
        __dest  = (unsigned short *) dest16->line[j];
        __img   = img8->line[j];
        __light = lum->line[j];

        for (i=0;i<len;i++)
        {
            *__dest++ = table[*__light++][*__img++];
        }

    }
}




// ###############################################################
//
void precalcLuminosity(COLOR_MAP8 table,PALETTE pal)
// 16 bits only !!
{
    int i,j;

    // initialize color table
    for ( i=0; i<256; i++)
    {
        int r,g,b;
        float sr,sb,sg,dr,dg,db;

        // unpack palette color integer
        r = pal[i].r*4;
        g = pal[i].g*4;
        b = pal[i].b*4;

        // original color shade
        table[128][i] = makecol16(r,g,b);

        // calculate shades to white
        sr=(float)r;
        sg=(float)g;
        sb=(float)b;
        dr = (float)(256 - r) / 128.0f; // 256
        dg = (float)(256 - g) / 128.0f; // 256
        db = (float)(256 - b) / 128.0f; // 256
        for ( j=0; j<127; j++) // 128
        {
            sr += dr;
            sg += dg;
            sb += db;
            table[129+j][i] = makecol16((int)sr,(int)sg,(int)sb);
        }

        // calculate shades to black
        sr = 0.0f;
        sg = 0.0f;
        sb = 0.0f;
        dr = (float)(r) / 128.0f;
        dg = (float)(g) / 128.0f;
        db = (float)(b) / 128.0f;
        for ( j=0; j<128; j++)
        {
            table[j][i] = makecol16((int)sr,(int)sg,(int)sb);
            sr += dr;
            sg += dg;
            sb += db;
        }
    }
}

// ###############################################################
//
void precalcTransparency(COLOR_MAP8 table,PALETTE pal1,PALETTE pal2)
// 16 bits only !!
{
    int i,j;

    // initialize color table
    for ( j=0; j<256; j++)
        for ( i=0; i<256; i++)
        {
            float r,g,b;

            // unpack palette color integer
            r = pal[i].r*4;
            g = pal[i].g*4;
            b = pal[i].b*4;

            r +=pal2[j].r*4;
            g +=pal2[j].g*4;
            b +=pal2[j].b*4;
            r = MID(0,r,255);
            g = MID(0,g,255);
            b = MID(0,b,255);
            table[j][i] = makecol16((int)r,(int)g,(int)b);
        }
}

// ###############################################################
//
void DrawColorPlasma(BITMAP * lum,int pos, double mul)
{
    unsigned char plasmax[320],plasmay[256];
    int i,j;

    for (i=0;i<320;i++)
        plasmax[i] =(63+fixtoi(
                         16*fsin(itofix(i+2*pos))+
                         16*fsin(itofix(mul*i+3*pos))+
                         16*fsin(itofix(-i+9*pos))+
                         16*fcos(itofix(-i+pos/7))));

    for (i=0;i<256;i++)
        plasmay[i] = (63+fixtoi(
                          16*fsin(itofix(2*pos-i))+
                          16*fcos(itofix(mul*i+pos))+
                          16*fsin(itofix(-i+pos/10))+
                          16*fsin(itofix(+i+7*pos))));


    for (j = lum->h-1;j >= 0;j--)
    {
        for (i=0;i<lum->w;i++)
        {
            lum->line[j][i]=plasmay[j]+plasmax[i];
        }

    }
}


void MotionBlurMono(BITMAP *bmp,float intensity,int minimum)
{
    unsigned char tabMBlur[256];
    int i,x,y;

    intensity = MID(1,intensity,255);
    for (i=0;i<256;i++)
    {
        x = i;
        x = x*(1-1/intensity);
        if (x<minimum) x=minimum;
        tabMBlur[i]=x;
    }

    {

        register unsigned char *p;
        for (y=bmp->h-1;y>=0;y--)
        {
            p = bmp->line[y];

            for (x=319;x>=0;x--)
            {
                *p=tabMBlur[*p];
                p++;
            }

        }

    }
}




// ###############################################################
//
void drawLight( BITMAP * buffer, BITMAP * img, BITMAP * light, int x, int y)
{
    int _x,_y,i,j,adder=0;
    int r1,r2,g1,g2,b1,b2,r,g,b;
    unsigned short * dest,* src, *blob;

    int clip_y1,clip_y2;

    if ((x<=0) || (x>=(SCREEN_W-light->w)) || (y<=0)|| (y>=SCREEN_H-light->h) /* || (y<=0)|| (y>=150)*/)
        return;

    clip_y2 = 58;
    clip_y1 = 0;
    /*
    if ((y<=0)&&(y>-50))
    {
        adder = 1-y;
        clip_y2 = 58-adder;
        clip_y1 = 0;
        y = 0;
    }
    if ((y>=140) && (y<=SCREEN_H)) clip_y2 = 58-(y-140);*/


    for (_y=clip_y1 ; _y<clip_y2 ; _y++)
    {
        dest = ((unsigned short*) buffer->line[_y+y])+x;
        src  = ((unsigned short*) img->line[_y+y])+x;
        blob = (unsigned short*) light->line[_y+adder];

        for (_x=0;_x<light->h;_x++)
        {
            i = *src;
            j = *blob;

            r1 = (i >> 11) & 0x1F;
            r2 = (j >> 11) & 0x1F;
            r = MIN(r1+r2,31);

            g1 = (i >> 5) & 0x3F;
            g2 = (j >> 5) & 0x3F;
            g = MIN(g1+g2,63);

            b1 = i & 0x1F;
            b2 = j & 0x1F;
            b = MIN(b1+b2,31);

            *dest = (r << 11) | (g << 5) | b;
            src++;
            blob++;
            dest++;
        }
    }

}


BITMAP* resize(BITMAP* src)
{
	BITMAP* tmp;
	tmp = create_bitmap(SCREEN_W, SCREEN_H);
	stretch_blit(src, tmp, 0, 0, src->w, src->h, 0, 0, 
		SCALE_X*src->w, SCALE_Y*src->h);
	return tmp;
}

RLE_SPRITE *resize_rle(RLE_SPRITE *src)
{
    BITMAP *tmp;
    BITMAP *tmp2;
	RLE_SPRITE *dest;
    int i;

    tmp = create_bitmap(src->w, src->h);
    tmp2 = create_bitmap(SCREEN_W, SCREEN_H);
    clear(tmp);
    draw_rle_sprite(tmp, src, 0, 0);
    stretch_blit(tmp, tmp2, 0, 0, src->w, src->h, 0, 0, 
		SCALE_X*src->w, SCALE_Y*src->h);
	dest = get_rle_sprite(tmp2);
	destroy_bitmap(tmp);
	destroy_bitmap(tmp2);
	return dest;
}

// ###############################################################
//
/* Precalculations pour l'effet des 3 ombres */
void PrecalkOmbre(void)
{
    int i;

    S2Back = resize(data[BFondLogo].dat);
    S2Logo = resize_rle(data[BLogo].dat);
    memcpy(S2BackPal,data[PFondLogo].dat,sizeof(PALETTE));
    halo1  = data[BHalo1].dat;
    halo2  = data[BHalo2].dat;
    halo3  = data[BHalo3].dat;
    S2Ombre = /*resize_rle*/(data[BLogoOmbre].dat);

    precalcLuminosity(S2tableLum,S2BackPal);
    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)-(i>>8)/3);
        if (c<0) c=0;
        S2tableOmbre.data[i>>8][i&255] = c;
    }
}


typedef struct TLightPosition
{
    // light coords
    int lx;
    int ly;
    // shadow coords
    int ox;
    int oy;
} TLightPosition;

void CalcLightPlace(struct TLightPosition * lp, int angle, int dist)
{
    lp->lx = SCALE_X*130+fixtoi(SCALE_X*dist*fcos(itofix(-angle)));
    lp->ly = SCALE_Y*70 +fixtoi(SCALE_Y*dist*fsin(itofix(-angle)));

    lp->ox = fixtoi(SCALE_X*dist*fcos(itofix(-angle+128)));
    lp->oy = fixtoi(SCALE_Y*dist*fsin(itofix(-angle+128)));
}

// ###############################################################
//
/* effet des 3 ombres */
void PartOmbre(void)
{
    BITMAP *lum;
    struct TLightPosition light[3];
    int partEnd=0;
    int lumFond=0;
    int radius=250;
    int opacity=0;
    int shadows=0;
    int alpha=0;
    int delta=0;
    int j;

    lum  = create_bitmap_ex(8,SCREEN_W,SCREEN_H);
    clear(buffer);
    clear(screen);
    color_map = &S2tableOmbre;

    while (!partEnd)
    {
        lumFond += VBLdelta;
        lumFond = MID(0,lumFond,127);

        Update(buffer);

        if ((GetPosition()<=0x0300))
            clear_to_color(lum,lumFond);

        CalcLightPlace(&light[0],VBLframe    ,radius);
        CalcLightPlace(&light[1],VBLframe+85 ,radius);
        CalcLightPlace(&light[2],VBLframe+170,radius);

        if (shadows)
        {
            draw_trans_rle_sprite(lum,S2Ombre,light[0].ox,light[0].oy);
            draw_trans_rle_sprite(lum,S2Ombre,light[1].ox,light[1].oy);
            draw_trans_rle_sprite(lum,S2Ombre,light[2].ox,light[2].oy);
        }
        mergeBitmap(buffer,S2Back,lum,S2tableLum);

        set_trans_blender(0,0,0,opacity);
        if ((GetPosition()<0x0300))
            draw_trans_rle_sprite(buffer,S2Logo,0,0);

        drawLight(buffer,buffer,halo1,light[0].lx,light[0].ly);
        drawLight(buffer,buffer,halo2,light[1].lx,light[1].ly);
        drawLight(buffer,buffer,halo3,light[2].lx,light[2].ly);

        if ((GetPosition()>=0x0100)&&(GetPosition()<=0x0200))
        {
            radius -= 4*VBLdelta;
            radius  = MID(50,radius,250);
        }
        if ((GetPosition()>=0x0200)&&(GetPosition()<=0x0300))
        {
            shadows = 1;
            opacity = 225+fixtoi(25*fsin(itofix(VBLframe*6)));
            radius  = 40+fixtoi(20*fsin(itofix(VBLframe*4)));
        }
        if ((GetPosition()==0x0300))
        {
            shadows=0;
            radius = 50;
            delta = VBLframe;
        }

        if ((GetPosition()>=0x0300))
        {
            int y;

            if (alpha<255)
                alpha=MID(0,VBLframe-delta,255);

            opacity = 255-alpha;
            set_trans_blender(0,0,0,alpha);

            for (y=0;y<SCREEN_H;y++)
            {
                int i;
                i = opacity*(opacity+y)/256;
                line(lum,0,y,SCREEN_W-1,y,MID(0,i,128));
            }

        }

        if ((GetPosition()==0x033F))
        {
            clear_to_color(lum,255);
        }
        if ((GetPosition()==0x0400))
        {
            partEnd=1;
        }

        if (key[KEY_ESC])
            partEnd=1;

    }
    destroy_bitmap(lum);
}

// ###############################################################
//


void putPixelLight( BITMAP * buffer,BITMAP * img,  int x, int y,int cr,int cg ,int cb )
{
    int i,j;
    int r1,r2,g1,g2,b1,b2,r,g,b;

    i = getpixel(img,x,y);
    j = makecol16(cr,cg,cb);

    r1 = (i >> 11) & 0x1F;
    r2 = (j >> 11) & 0x1F;
    r = MID(0,r1+r2,31);

    g1 = (i >> 5) & 0x3F;
    g2 = (j >> 5) & 0x3F;
    g = MID(0,g1+g2,63);

    b1 = i & 0x1F;
    b2 = j & 0x1F;
    b = MID(0,b1+b2,31);

    putpixel(buffer,x,y,(r << 11) | (g << 5) | b);
}




void genererParticule(int x,int y,int cr,int cg,int cb)
{
    int i;

    for (i=2*VBLdelta;i>=0;i--)
    {
        particule[partMax].x=x+rand()%10;
        particule[partMax].y=y+rand()%10;
        particule[partMax].cr=cr;
        particule[partMax].cg=cg;
        particule[partMax].cb=cb;
        partMax++;
    }
}

void calculParticule(BITMAP *buffer,BITMAP *dest)
{
    int x;
    for (x=partDeb;x<partMax;x++)
    {
        if (particule[x].cr>0)particule[x].cr-=VBLdelta*6;
        if (particule[x].cg>0)particule[x].cg-=VBLdelta*6;
        if (particule[x].cb>0)particule[x].cb-=VBLdelta*6;
        if (particule[x].cr<0)particule[x].cr=0;
        if (particule[x].cg<0)particule[x].cg=0;
        if (particule[x].cb<0)particule[x].cb=0;
        if (particule[x].y<199)particule[x].y+=particule[x].g;
        particule[x].g+=0.004* (double) VBLdelta;
        if ((particule[x].cr==0)&&(particule[x].cg==0)&&(particule[x].cb==0))
            partDeb++;

        putPixelLight(dest,buffer,particule[x].x  ,particule[x].y  ,particule[x].cr  ,particule[x].cg  ,particule[x].cb);
        putPixelLight(dest,buffer,particule[x].x+1,particule[x].y  ,particule[x].cr/DIVISEUR,particule[x].cg/DIVISEUR,particule[x].cb/DIVISEUR);
        putPixelLight(dest,buffer,particule[x].x-1,particule[x].y  ,particule[x].cr/DIVISEUR,particule[x].cg/DIVISEUR,particule[x].cb/DIVISEUR);
        putPixelLight(dest,buffer,particule[x].x  ,particule[x].y-1,particule[x].cr/DIVISEUR,particule[x].cg/DIVISEUR,particule[x].cb/DIVISEUR);
        putPixelLight(dest,buffer,particule[x].x  ,particule[x].y+1,particule[x].cr/DIVISEUR,particule[x].cg/DIVISEUR,particule[x].cb/DIVISEUR);
    }
}

void PrecalkLumiereParticule()
{
    img_fond  = data[BKnightsBack].dat;
    img_halor = data[BHalo1].dat;
    img_haloj = data[BHalo2].dat;
    img_halob = data[BHalo3].dat;
    img_code  = data[BCode].dat;
    img_code2 = data[BCode2].dat;
    img_gfx   = data[BGfx].dat;
    img_music = data[BMusic].dat;
}

void PartLumiereParticule()
{

    int xr,xb,xj,yr,yb,yj,etat=0;
    double toto=0,totox=0,totoy=0,i=0,j=0;
    double num1=0,num2=0,num3=0,num4=0;
    double d1=2,d2=0,d3=0,d4=0;

    set_palette(palettefond);
    etat=1;
    do
    {

        if (etat==1 )
        {
            toto+=fVBLdelta/2;
            if (toto>150) etat=2;
        }
        if (etat==2  )
        {
            toto-=fVBLdelta/2;
            if (toto<0)etat=3;
        }
        if (etat==3)
        {
            if (totox<120)totox+=fVBLdelta/2;
            if (totoy<60)totoy+=fVBLdelta/4;
        }

        j=((double)VBLframe)/70.0;
        i=cos(j)*4;

        xr=cos(i)*(120-toto-totox)+130;
        yr=sin(i)*(60-toto-totoy)+70-(totoy/3);

        xb=cos(i+90)*(120-toto-totox)+130;
        yb=sin(i+90)*(60-toto-totoy)+70-(totoy/3);

        xj=cos(i+180)*(120-toto-totox)+130;
        yj=sin(i+180)*(60-toto-totoy)+70-(totoy/3);

        genererParticule(xb+25,yb+25,255,128,128);
        genererParticule(xr+25,yr+25,128,128,255);
        genererParticule(xj+25,yj+25,255,255,0);

        blit(img_fond,buffer,0,0,0,0,320,200);
        calculParticule(buffer,buffer);
        drawLight(buffer,buffer,img_halor,xr,yr);
        drawLight(buffer,buffer,img_halob,xb,yb);
        drawLight(buffer,buffer,img_haloj,xj,yj);

        if (num1>240)
        {
            d1 = -fVBLdelta*1.5;
            d2 =  fVBLdelta*1.5;
        }
        if (num2>240)
        {
            d2 = -fVBLdelta*1.5;
            d3 =  fVBLdelta*1.5;
        }
        if (num3>240)
        {
            d3 = -fVBLdelta*1.5;
            d4 =  fVBLdelta*1.5;
        }
        if (num4>240)
        {
            d4 = -fVBLdelta*1.5;
        }

        num1+=d1;
        num2+=d2;
        num3+=d3;
        num4+=d4;

        num1 = MID(0,num1,255);
        num2 = MID(0,num2,255);
        num3 = MID(0,num3,255);
        num4 = MID(0,num4,255);

        set_trans_blender(0,0,0,num1);
        draw_trans_sprite(buffer,img_code,10,10);
        set_trans_blender(0,0,0,num2);
        draw_trans_sprite(buffer,img_code2,190,10);
        set_trans_blender(0,0,0,num3);
        draw_trans_sprite(buffer,img_gfx,0,120);
        set_trans_blender(0,0,0,num4);
        draw_trans_sprite(buffer,img_music,160,120);

        Update(buffer);


    } while (GetPosition()<0x0710);
}



// ###############################################################
//
/*
typedef struct TGridItem
{
  int u,v,c;
}TGridItem;

typedef struct TGridItem TGrid[41][26];

unsigned char * expandGrid_lum;
unsigned char expandGrid_texture[200][320];
void expandGrid(TGrid g,BITMAP * bmp,BITMAP * lum,BITMAP * texture)
// 320x200x16 !!!
{
  int x,y,i,j,u,v;
  unsigned short c;
  int ul,ur,vl,vr;
  int ud,vd;
  unsigned char *dest,*src;



  for (y=0;y<25;y++)
    for (x=0;x<40;x++)
    {
      vl= g[x][y].v;
      vr= g[x][y+1].v;
      vd= (vr-vl)*32;
      v = vl*256;

      for (j=0;j<8;j++)
      {
        int x__v;
        ul= g[x][y].u;
        ur= g[x+1][y].u;
        ud= (ur-ul)*32;
        u = ul*256;

        v += vd;

        x__v = (v>>8)&255;
        dest= ((unsigned char *) bmp->line[y*8+j]+x*8);

//        expandGrid_texture = dest;

        // reorganiser en lignes horizontales de 320...
        asm volatile("
          .rept 8
            addl  %%ecx,%%edx
            movb  %%dh,%%al
            movb  %%bl,%%ah
            movb  (%%esi,%%eax),%%al
            movb  %%al,(%%edi)
            addl  $1,%%edi
          .endr
        ":
        : "S" (texture->dat),"D" (dest),"d" (u),"c" (ud),"b"(x__v)
        :"eax","ebx","ecx","edx","edi","esi");

//        for (i=0;i<8;i++)
//        {
//          u += ud;
////          c = *(((unsigned short *) texture->line[(v/256)&255])+((u/256)&255));
//          c = *(((unsigned short *) texture->dat+(( (v&0xff00)+(u/256))) ));
//          *dest++=c;
//        }
      }
    }
}


void testGrid(void)
{
  TGrid grille;
  int x,y,c;
  BITMAP *texture,*buffer8,*lum;
  PALETTE pal;


  buffer8 = create_bitmap_ex(8,320,200);
  lum = create_bitmap_ex(8,320,200);
  clear_to_color(lum,128);
  set_color_conversion(COLORCONV_NONE);
  texture = load_bitmap("gfx/texture3.pcx",pal);
  set_palette(pal);
  precalcLuminosity(tablePlasma,pal);

  for (y=0;y<200;y++)
    for (x=0;x<320;x++)
    expandGrid_texture[y][x] = getpixel(texture,x,y);

  while(!key[KEY_ESC])
  {

    for (y=0;y<26;y++)
      for (x=0;x<41;x++)
      {
        grille[x][y].u=128+64*cos(((8*x+VBLframe)*3.14/256))+64*cos(((8*x+VBLframe/4)*3.14/256));
        grille[x][y].v=128+64*sin(((8*y+VBLframe)*3.14/256))+64*cos(((VBLframe*8)*3.14/256));
//        grille[x][y].c=128+64*sin(((8*y+2*VBLframe)*3.14/256))+64*cos(((VBLframe*7)*3.14/256));
      }

    clear_to_color(lum,mouse_y);
    expandGrid(grille,buffer8,lum,texture);
    mergeBitmap(buffer,buffer8,lum,tablePlasma);
    WaitVBL();
    blit(buffer,screen,0,0,0,0,320,200);
  }
}
*/

// ###############################################################
//
void PrecalkTunnel(void)
{
    int x,y,i;
    PALETTE pal2;

    memcpy(pal,data[PPal1].dat,sizeof(pal));
    precalcLuminosity(tablePlasma,pal);

    memcpy(pal,data[PFeu].dat,sizeof(pal));
    precalcLuminosity(tablePlasmaBump,pal);

    memcpy(pal,data[PPal2].dat,sizeof(pal));
    precalcLuminosity(tableGuru,pal);

    memcpy(pal2,data[Pmap8A].dat,sizeof(pal));
//  memcpy(pal2,data[PPlasma].dat,sizeof(pal));
    memcpy(pal,data[PPal1].dat,sizeof(pal));
    precalcTransparency(table3D,pal,pal2);

    memcpy(pal,data[Pmap8A].dat,sizeof(pal));
    precalcLuminosity(tableTunnel,pal);
    SetTunnelMap(data[Bmap8A].dat);
    CalcTunnelLookup( lookup1, 0, 1);
    CalcTunnelLookup( lookup2, 1, 0);

    K3DInitObject(&O3DTrois);
    K3DLoadObjectASC("styler2.dat#ASCtrois", &O3DTrois);
//  K3DExtractObjectASC(data[ASCtrois].dat, &O3DTrois);
    lum640 = create_bitmap_ex(8,640,400);

    for (y=0;y<400;y++)
        for (x=0;x<640;x++)
        {
            int r;
            r = (int)sqrt((x-320)*(x-320)+(y-200)*(y-200));
            r = (r*256)/150;
            r = MID(0,r,128);
            _putpixel(lum640,x,y,255-r);
        }

    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>8));
        tableGlenz8A.data[i>>8][i&255] = MID(0,c,255);
    }
    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>9));
        tableGlenz8B.data[i>>8][i&255] = MID(0,c,255);
    }
    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>10));
        tableGlenz8C.data[i>>8][i&255] = MID(0,c,255);
    }
    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>11));
        tableGlenz8D.data[i>>8][i&255] = MID(0,c,255);
    }

    for (i = 0xffff;i >= 0;i--)
    {
        int c,a,b;

        a = (i&255);
        b = (i>>8);

        c = sqrt(a*a+b*b/4);

        tableMetaballs.data[i>>8][i&255] = MID(0,c,255);
    }
}
// ###############################################################
//
void PartTunnel()
{
    int i,force;
    BITMAP * lum;
    int ylum=85;
    int tx,ty;
    int ro=10;

    void drawTunnelFrame(BITMAP * buffer)
    {

        force   -= 2*VBLdelta;
        if ((mouse_b&1))
            force += 7*VBLdelta;
        force = MID(0,force,40);
        deform.intensity = force;

        DoDeformTable(&deform, VBLframe);

        blit(lum640,lum,160,100,0,0,640,400);
        DrawTunnel(buffer8, lookup1, 0, 0, VBLframe*2, VBLframe, &deform);
        mergeBitmap(buffer,buffer8,lum,tableTunnel);

    }

    void drawParticleFrame(BITMAP * buffer)
    {
        int xr,yr,xb,yb,xj,yj;

        int angle,dist;

        angle = VBLframe;
        dist = ro/5;
        // new position formula ...

        xr=160 -30+fixtoi(1.3*dist*fcos(itofix(angle)));
        yr=ylum-30+fixtoi(dist*fsin(itofix(angle)));
        xb=160 -30+fixtoi(1.3*dist*fcos(itofix(angle+85)));
        yb=ylum-30+fixtoi(dist*fsin(itofix(angle+85)));
        xj=160 -30+fixtoi(1.3*dist*fcos(itofix(angle+170)));
        yj=ylum-30+fixtoi(dist*fsin(itofix(angle+170)));


        genererParticule(xr+25,yr+25,255,0,0);
        genererParticule(xb+25,yb+25,0,0,255);
        genererParticule(xj+25,yj+25,255,255,0);

        blit(data[BKnightsBack].dat,buffer,0,0,0,0,320,200);
        calculParticule(buffer,buffer);
        drawLight(buffer,buffer,data[BHalo3].dat,xr,yr);
        drawLight(buffer,buffer,data[BHalo1].dat,xb,yb);
        drawLight(buffer,buffer,data[BHalo2].dat,xj,yj);
    }

    lum = create_bitmap_ex(8,320,200);
    K3DSetObjectRendering(&O3DTrois, FLAT, NONE, NONE);
    set_projection_viewport(0,0,320,200);
    O3DTrois.zmax=400;

    ylum = 85;

    deform.param[0].freq=0x20000;
    deform.param[0].phase=0;
    deform.param[1].freq=0x20000;
    deform.param[1].phase=0;
    deform.param[2].freq=0x20000;
    deform.param[2].phase=0;
    deform.param[3].freq=0x20000;
    deform.param[3].phase=0;

    K3DPlaceObject(&O3DTrois, 0, 0, itofix(160), 0, 0, 0);
    K3DRotateObject(&O3DTrois);
    K3DProjectObject(&O3DTrois);

    for (i=0;i<O3DTrois.faces;i++)
    {
        int r,g,b;
        r = O3DTrois.projected[O3DTrois.face[i].v1].z>>16;
        g = O3DTrois.projected[O3DTrois.face[i].v1].x>>16;
        b = O3DTrois.projected[O3DTrois.face[i].v1].y>>16;
//    O3DTrois.face[i].color = makecol16(MID(128,r,255),MID(0,g,255),MID(128,b,255));
        O3DTrois.face[i].color = makecol16(MID(0,r,0),MID(200,g,255),MID(0,b,0));
    }

    ro=0;
    WaitVBL();
    while (GetPosition()<0x0800)
    {
        Update(buffer);

        if (ylum<100)
            ylum += VBLdelta/8.0;
        ylum = MID(85,ylum,100);
        set_projection_viewport(0,ylum-100,320,200);

        ro += VBLdelta*2;

        ro = MID(1,ro,600);
        if (ro>300)
        {
            if (GetPosition()&2)
                drawTunnelFrame(buffer);
            else
                drawParticleFrame(buffer);
        }
        else
            drawParticleFrame(buffer);

        K3DPlaceObject(&O3DTrois, 0, 0, itofix(1600-ro*2), 0, 0, (235-VBLframe)<<16);
        drawing_mode(DRAW_MODE_TRANS,0,0,0);
        set_trans_blender(0,0,0,64);
        K3DRenderObject(buffer, &O3DTrois);
    }
    ro =0;
    WaitVBL();

    color_map=&tableGlenz8B;

    deform.param[1].freq=0x10000;
    deform.param[2].freq=0x30000;
    deform.param[2].phase=0;
    deform.param[3].freq=0x10000;
    deform.param[3].phase=0;

    while (GetPosition()<0x0b00)
    {
        Update(buffer);

        // intensity of the tunnel deformations
        force = 35+20*sin(VBLframe*2*3.14/473);

        // deformations
        deform.param[2].phase=VBLframe*3;
        deform.param[3].phase=deform.table[VBLframe&255];
        deform.intensity = force;
        DoDeformTable(&deform, VBLframe);

        // move the center of the tunnel
        if (GetPosition()<0x0900)
            ro+=VBLdelta;
        ro = MID(0,ro,50);

        tx=80+fixtoi(1.6*(ro/2)*fsin(itofix(VBLframe)));
        ty=50+fixtoi((ro/2)*fcos(itofix(VBLframe)));

        // light of the tunnel
        blit(lum640,lum,160-tx,100-ty,0,0,640,400);

        // the "Styler rmx" picture
        if (GetPosition()<0x0810)
            if ((GetPosition()+1)&2)
//       draw_trans_rle_sprite(lum,data[BRmxStyler].dat,0,0);
                blit(data[BRmxStyler].dat,lum,0,0,0,0,320,200);

        // da tunnel
        DrawTunnel(buffer8, lookup1, tx, ty, VBLframe*5, fixtoi(255*fsin(itofix(VBLframe/2))), &deform);

        if (GetPosition()>=0x0900)
        {
            // the 3D stuff...
            int size;

            // one
            for (i=0;i<O3DTrois.faces;i++)
                O3DTrois.face[i].color = 50;
            size = 40+40*sin(VBLframe*2*3.14/256);
            set_projection_viewport(0-size/2,0-size/2,320+size,200+size);
            K3DPlaceObject(&O3DTrois, 0, 0, itofix(400), 40*fsin(itofix(3*VBLframe)), itofix(255-ty/8), (235-VBLframe)<<16);
            drawing_mode(DRAW_MODE_SOLID,0,0,0);
            K3DRenderObject(lum, &O3DTrois);

            // and two
            for (i=0;i<O3DTrois.faces;i++)
                O3DTrois.face[i].color = 180;
            set_projection_viewport(20,20,280,160);
            K3DPlaceObject(&O3DTrois, 0, 0, itofix(400), 40*fsin(itofix(3*VBLframe)), itofix(255-ty/8), (235-VBLframe)<<16);
            drawing_mode(DRAW_MODE_SOLID,0,0,0);
            K3DRenderObject(lum, &O3DTrois);

            // little "Styler rmx"
            if ((GetPosition()+1)&2)
                draw_trans_rle_sprite(lum,data[BstylerCoin].dat,0,0);
        }
        else
        {
            int size;

            for (i=0;i<O3DTrois.faces;i++)
                O3DTrois.face[i].color = 64;
            size = 40+40*sin(VBLframe*2*3.14/256);
            set_projection_viewport(0-size/2,0-size/2,320+size,200+size);
            K3DPlaceObject(&O3DTrois, 0, 0, itofix(500), (VBLframe)<<16, (VBLframe)<<15, (2*VBLframe)<<16);
            drawing_mode(DRAW_MODE_TRANS,0,0,0);
            color_map=&tableGlenz8B;
            K3DRenderObject(lum, &O3DTrois);
        }
        mergeBitmap(buffer,buffer8,lum,tableTunnel);
    }
}


// ###############################################################
//   PART --> Mega-Bump <--
BITMAP *bumpMap;
BITMAP *bumpBumped;
BITMAP *bumpSpot;
PALETTE palLogo;


void PrecalkBump(void)
{
    int x,y;

    bumpBumped = create_bitmap_ex(8,640, 480);
    set_color_depth(8);

    memcpy(palLogo,data[PBump].dat,sizeof(PALETTE));
    bumpSpot = data[BHalo].dat;
    bumpMap  = data[BBump].dat;

    for ( y=0;y<480;y++)
        for ( x=0;x<640;x++)
            putpixel(bumpBumped,x,y,palLogo[getpixel(bumpMap,x,y)].b*4);

    InitBumpTable(&bumpTable,1);
    SetBumpTable(&bumpTable, bumpBumped);
    clear_to_color(bumpTable.lightMap,128);
    precalcLuminosity(tableLogo,palLogo);

    destroy_bitmap(bumpBumped);
    /////////////////////////////////////

    K3DInitObject(&O3DSalle);
    K3DInitObject(&O3DCity);
    K3DInitObject(&O3DGisquet);
    K3DInitObject(&O3DAutre);

    K3DLoadObjectASC("styler2.dat#ASCcity", &O3DCity);
    K3DLoadObjectASC("styler2.dat#ASCsalle", &O3DSalle);
    K3DLoadObjectASC("styler2.dat#ASCgisquet", &O3DGisquet);
    K3DLoadObjectASC("styler2.dat#ASCautre", &O3DAutre);
}


void PartBump(void)
{
    BITMAP *lum;
    BITMAP *dirtyMap;
    int i,max,bx=0,by=0,x,y;

    // nb of spots
    max = 1;

    lum = create_bitmap_ex(8,320,200);

    clear_to_color(lum,128);
    set_palette(palLogo);

    color_map = &tableGlenz8B;



    dirtyMap = create_bitmap_ex(8,320,200);



    while (GetPosition()<0x0e00)
    {
        static int blocked=0;
        // bump position
        bx =  320+190*cos((VBLframe)*3.14*2/1300)*cos((VBLframe)*3.14*2/500);
        by =  200+72*sin((VBLframe)*3.14987*2/1500)*cos((VBLframe)*3.14*2/700);


        if ((GetPosition()>=0x0c00) && (GetPosition()<0x0e00))
            if (((GetPosition()&255)%20)==0)
            {
                if (!blocked)
                    max+=2;
                blocked=1;
            }
            else
                blocked=0;

        for (i=0;i<max;i++)
        {
            int x,y,d;

            d = i*117;

            // draw the wonderful spots
            x = 24
                *(cos((VBLframe+d)*3.14*2/200)
                  +cos((VBLframe+d)*3.14*2/120)
                  +cos((VBLframe+d)*3.14*2/100)
                  +cos((VBLframe+d)*3.14*2/2800));

            y = 24
                *(sin((VBLframe+d)*3.14*2/200)
                  +sin((VBLframe+d)*3.14*2/1400)
                  +sin((VBLframe+d)*3.14*2/140)
                  +sin((VBLframe+d)*3.14*2/400));
            draw_trans_sprite(bumpTable.lightMap,bumpSpot,128+y-32,128+x-32);
        }




        x = bx-192;
        y = by-100;

        blit(data[BBump].dat,dirtyMap,x,y,0,0,320,200);

        // bump + merging
        DrawBumpEx(lum, 160, 100, &bumpTable,x+32,y);


        mergeBitmap(buffer,dirtyMap,lum,tableLogo);

        draw_rle_sprite(buffer,data[BMbump].dat,0,0);

        if ( (GetPosition() == 0x0d34) ||
                (GetPosition() == 0x0d38) ||
                (GetPosition() == 0x0d3C) )
        {
            clear_to_color(buffer,0xffff);
        }

        // synchro + stuffs
        rect(buffer,0,0,319,199,0);
        rect(buffer,1,1,318,198,0);
        Update(buffer);

        clear_to_color(bumpTable.lightMap,128);

        if (key[KEY_ESC])
            break;
    }

    destroy_bitmap(lum);
}


void testRubber(void)
{
    BITMAP * rubber;
    int x,y,i,j;

    rubber = create_bitmap_ex(16,64,128);
    clear(buffer);
    clear_to_color(rubber,makecol16(0,0,255));

    for (j=0;j<64;j++)
    {
        int c;

        c = 127+fixtoi(127*fcos(itofix(j*2)));

        line(rubber,0,j,j,j, makecol16(255-c,0,0));
        line(rubber,j,j,63,j,makecol16(c,0,0));
        x=0+3+fixtoi(3*fcos(itofix(j*4)));
        line(rubber,-1 ,j,x-1,j,makecol16(0,0,200));
        x=63-3+fixtoi(3*fcos(itofix(j*4+128)));
        line(rubber,64,j,x+1,j,makecol16(0,0,200));
    }

    for (y=0;y<128;y++)
        blit(rubber,buffer,0,(y&63),160,y,64,1);

    while (!key[KEY_ESC])
    {
        int y;

        WaitVBL();
        blit(buffer,screen,0,0,0,0,320,200);

        for (y=0;y<200;y++)
        {
            i = fixtoi(60*fcos(itofix(VBLframe+y/2)))+fixtoi(100*fsin(itofix(VBLframe/2+y/1)));

            blit(rubber,buffer,0,(i&63),0,y,64,1);
        }

        while (key[KEY_F1])
            {}
    }

}


typedef struct K3DParticule
{
    fixed x,y,z;
    unsigned char lum;
}K3DParticule;

typedef struct K3DParticuleSystem
{
    struct K3DParticule * particule;
    struct K3DParticule * rotated;
    struct K3DParticule * projected;
    int                   particules;
    int                   type;
    fixed                 x,y,z;
    fixed                 rx,ry,rz;
    fixed                 scale;
}K3DParticuleSystem;

void K3DParticuleInit(struct  K3DParticuleSystem* ps,int num)
{
    ps->particule  = malloc(num*sizeof(K3DParticule));
    ps->rotated    = malloc(num*sizeof(K3DParticule));
    ps->projected  = malloc(num*sizeof(K3DParticule));
    ps->particules = num;
    ps->type       = 0;
    ps->x=ps->y=ps->z=ps->rx=ps->ry=ps->rz = itofix(0);
    ps->scale = itofix(1);
}

void K3DParticuleDelete(struct  K3DParticuleSystem* ps)
{
    if (ps->particule == NULL)
        free(ps->particule);
    if (ps->rotated == NULL)
        free(ps->rotated);
    if (ps->projected == NULL)
        free(ps->projected);

    ps->particules = 0;
    ps->type       = 0;
}

void K3DParticuleRotate(struct  K3DParticuleSystem* ps)
{
    MATRIX matrix;
    int i;

    get_transformation_matrix(&matrix, ps->scale, ps->rx, ps->ry, ps->rz,
                              ps->x , ps->y , ps->z);
    for (i = 0;i < ps->particules;i++)
    {
        apply_matrix(&matrix, ps->particule[i].x, ps->particule[i].y, ps->particule[i].z,
                     &ps->rotated[i].x, &ps->rotated[i].y, &ps->rotated[i].z);
        ps->rotated[i].lum = ps->particule[i].lum;
    }
}

void K3DParticuleProject(struct  K3DParticuleSystem* ps)
{
    int i;

    for (i = 0;i < ps->particules;i++)
    {
        persp_project(ps->rotated[i].x  ,  ps->rotated[i].y  , ps->rotated[i].z,
                      &ps->projected[i].x, &ps->projected[i].y);
    }
}

void K3DParticulePixel(BITMAP * buffer,struct  K3DParticuleSystem* ps)
{
    int i;

    for (i = 0;i < ps->particules;i++)
    {
        if (fixtoi(ps->rotated[i].z)>5)
        {
            unsigned int c;
            c = 255-(fixtoi(ps->rotated[i].z))/8;
            c *= 2;
            c = MID(0,c,255);

            putpixel(buffer,fixtoi(ps->projected[i].x),fixtoi(ps->projected[i].y),c);
            c /= 2;
            putpixel(buffer,fixtoi(ps->projected[i].x)-1,fixtoi(ps->projected[i].y),c);
            putpixel(buffer,fixtoi(ps->projected[i].x),fixtoi(ps->projected[i].y)-1,c);
            putpixel(buffer,fixtoi(ps->projected[i].x)+1,fixtoi(ps->projected[i].y),c);
            putpixel(buffer,fixtoi(ps->projected[i].x),fixtoi(ps->projected[i].y)+1,c);
        }
    }
}

void K3DParticuleDraw(BITMAP * buffer,struct  K3DParticuleSystem* ps)
{
    int i;

    for (i = 0;i < ps->particules;i++)
    {
        int c,x,y;
        // tableGlenz8;
        unsigned char *p;

        if (fixtoi(ps->rotated[i].z)>5)
        {

            x = fixtoi(ps->projected[i].x);
            y = fixtoi(ps->projected[i].y);
            if (x>0 && y>0 && x<318 && y<198)
            {
//      c = MID(0,fixtoi(ps->rotated[i].z),255);
                c = 130;//-(c);

                p = buffer->line[y]+x;
                // the main square
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p--;
                *p = tableGlenz8A.data[c][*p];
                // the borders
                c /=2;
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p-=1+320*3;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320+1;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p-=320+3;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];

            }

            x = 10+fixtoi(ps->projected[i].x);
            y = 10+fixtoi(ps->projected[i].y);
            if (x>0 && y>0 && x<318 && y<198)
            {
//      c = MID(0,fixtoi(ps->rotated[i].z),255);
                c = 120;//-(c);

                p = buffer->line[y]+x;
                // the main square
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p--;
                *p = tableGlenz8A.data[c][*p];
                // the borders
                c /=2;
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p-=1+320*3;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320+1;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p-=320+3;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];

            }

            x = fixtoi(ps->projected[i].x);
            y = -10+fixtoi(ps->projected[i].y);
            if (x>0 && y>0 && x<318 && y<198)
            {
//      c = MID(0,fixtoi(ps->rotated[i].z),255);
                c = 120;//-(c);

                p = buffer->line[y]+x;
                // the main square
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p--;
                *p = tableGlenz8A.data[c][*p];
                // the borders
                c /=2;
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p-=1+320*3;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320+1;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p-=320+3;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];

            }

            x = fixtoi(ps->projected[i].x);
            y = 20+fixtoi(ps->projected[i].y);
            if (x>0 && y>0 && x<318 && y<198)
            {
//      c = MID(0,fixtoi(ps->rotated[i].z),255);
                c = 120;//-(c);

                p = buffer->line[y]+x;
                // the main square
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p--;
                *p = tableGlenz8A.data[c][*p];
                // the borders
                c /=2;
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p-=1+320*3;
                *p = tableGlenz8A.data[c][*p];
                p++;
                *p = tableGlenz8A.data[c][*p];
                p+=320+1;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];
                p-=320+3;
                *p = tableGlenz8A.data[c][*p];
                p+=320;
                *p = tableGlenz8A.data[c][*p];

            }
        }
    }
}

void K3DParticuleRender(BITMAP * buffer,struct  K3DParticuleSystem* ps)
{
    K3DParticuleRotate(ps);
    K3DParticuleProject(ps);
    K3DParticuleDraw(buffer,ps);
}

void PartParticules()
{
    BITMAP * map,*lum;
    struct  K3DParticuleSystem ps;

    int i,alpha=-128;
    int sobj;
    int nb;
    int dist=0;

    sobj = 2700;
    nb=sobj*1;


    map = create_bitmap_ex(8,320,200);
    lum = create_bitmap_ex(8,320, 200);
    clear(lum);

    K3DParticuleInit(&ps,nb);
    for (i=0;i<nb;i++)
    {
        ps.particule[i].x   = itofix(100-rand()%200);
        ps.particule[i].y   = itofix(100-rand()%200);
        ps.particule[i].z   = itofix(100-rand()%200);
        ps.particule[i].lum = i;
    }

    set_projection_viewport(0,0,320,200);

    while (GetPosition() < 0x1100)
    {
        int zoom;

        Update(buffer);

        MotionBlurMono(lum,6,64);
        if (GetPosition() > 0x1030)
        {
            ThresholdSmoothMono(lum,data[BTransition2].dat, alpha , 100, 64 , 255);
            alpha += 4*VBLdelta;
        }
//   else

        ps.rx=VBLframe<<15;
        ps.ry=VBLframe<<14;

        ps.z=itofix(dist);

        K3DParticuleRotate(&ps);

        for (i=0;i<nb;i++)
        {
            ps.rotated[i].x   += 15*fcos(itofix(VBLframe*4)+ps.rotated[i].y);
            ps.rotated[i].z   += 10*fsin(itofix(VBLframe*5)+2*ps.rotated[i].y);
        }

        K3DParticuleProject(&ps);
        K3DParticuleDraw(lum,&ps);

        if ((GetPosition() >= 0x0f00) && (GetPosition() < 0x1000))
        {
            dist += 2*VBLdelta;
            dist = MID(0,dist,250);
            if (GetPosition()&2)
                draw_rle_sprite(lum,data[BFrime].dat,0,0);
        }

        zoom = 200+200*fabs(sin(VBLframe*2*3.14/700.0));
        stretch_blit(data[BBump].dat,map,0,0,640,480,160-zoom*0.8,100-zoom/2,zoom*1.6,zoom);

        mergeBitmap(buffer,map,lum,tableLogo);

        while (key[KEY_F1])
            {}

        if (key[KEY_ESC])
            break;
    }
    K3DParticuleDelete(&ps);
    destroy_bitmap(lum);
}

void drawBigParticule(BITMAP * buffer,int x, int y, unsigned char c)
{
    if (x>0 && y>0 && x<318 && y<198)
    {
        unsigned char * p;
        p = buffer->line[y]+x;
        // the main square
        *p = tableGlenz8A.data[c][*p];
        p++;
        *p = tableGlenz8A.data[c][*p];
        p+=320;
        *p = tableGlenz8A.data[c][*p];
        p--;
        *p = tableGlenz8A.data[c][*p];
        // the borders
        c /=2;
        p+=320;
        *p = tableGlenz8A.data[c][*p];
        p++;
        *p = tableGlenz8A.data[c][*p];
        p-=1+320*3;
        *p = tableGlenz8A.data[c][*p];
        p++;
        *p = tableGlenz8A.data[c][*p];
        p+=320+1;
        *p = tableGlenz8A.data[c][*p];
        p+=320;
        *p = tableGlenz8A.data[c][*p];
        p-=320+3;
        *p = tableGlenz8A.data[c][*p];
        p+=320;
        *p = tableGlenz8A.data[c][*p];
    }
}

void testFollets(void)
{
    BITMAP * map,*lum;
    int x,y;
    int i;
    int nb=256;
    unsigned char tabMBlur[256];

    for (i=0;i<256;i++)
    {
        x = i;
        x = x*0.9;
        if (x<0) x=0;
        tabMBlur[i]=x;
    }

    map = data[BBump].dat;
    lum = create_bitmap_ex(8,320, 200);
    clear(lum);


    for (;;)
    {

        Update(buffer);
        {
            register unsigned char *p;
            for (y=199;y>=0;y--)
            {
                p = lum->line[y];
                for (x=319;x>=0;x--)
                {
                    *p=tabMBlur[*p];
                    p++;
                }
            }
        }

        for (i=1;i<nb;i++)
        {
            int x,y,r;

            r = 20+20*(cos((i+VBLframe*4)*2*3.14/256)*
                       sin((i/8+VBLframe/10)*2*3.14/256)+
                       sin((i*2+VBLframe/140)*2*3.14/256)*
                       sin((i/7+VBLframe/440)*2*3.14/32)*
                       sin((i+VBLframe*cos((i+VBLframe)*2*3.14/256))*2*3.14/256)
                      );

            x = 160+
                30*cos((i+VBLframe/517)*2*3.14/256)+
                20*sin((VBLframe/43)*2*3.14/124)
                +(r)*(
                    cos((i+VBLframe/150)*2*3.14/256)+
                    sin((i+VBLframe)*2*3.14/128)
                );
            y = 100+
                20*sin((i+VBLframe/154)*2*3.14/256)+
                15*sin((i+VBLframe*3)*2*3.14/512)
                +(r)*(
                    sin((i+VBLframe/300)*2*3.14/256)+
                    sin((i/2+VBLframe)*2*3.14/113)
                );

            drawBigParticule(lum,x,y,255);
        }

        mergeBitmap(buffer,data[BFondLogo].dat,lum,S2tableLum);

        while (key[KEY_F1])
            {}

        if (key[KEY_ESC])
            break;
    }
}


void Draw3DWaves(BITMAP *lum, fixed dist, fixed rx, fixed ry, fixed rz)
{
#define gx 24
#define gy 24
    V3D grille[gx][gy];
    V3D rotated[gx][gy];
    V3D projected[gx][gy];
    int x,y;
    MATRIX matrix;

    for (y=0;y<gx;y++)
        for (x=0;x<gx;x++)
        {

            grille[x][y].x=itofix(x-gx/2)*20;
            grille[x][y].y=itofix(y-gy/2)*20;
            grille[x][y].z=60*(fsin(itofix(x*16+3*VBLframe))+fcos(itofix(y*16+VBLframe))
                               +fcos(itofix(y*20+2*VBLframe))+fcos(itofix(x*11+VBLframe/5)));
        }

    get_transformation_matrix(&matrix, itofix(1), rx, ry, rz, 0, 0, dist);

    for (y=0;y<gx;y++)
        for (x=0;x<gx;x++)
        {
            if (grille[x][y].z>0)
            {
                apply_matrix(&matrix ,grille[x][y].x ,grille[x][y].y ,grille[x][y].z,
                             &rotated[x][y].x ,&rotated[x][y].y ,&rotated[x][y].z);
                persp_project(rotated[x][y].x ,rotated[x][y].y ,rotated[x][y].z,
                              &projected[x][y].x,&projected[x][y].y);
            }
        }

    for (y=0;y<gx-1;y++)
        for (x=0;x<gx-1;x++)
        {
            if ((grille[x][y].z>0)&&(rotated[x][y].z>0x100000)&&(rotated[x+1][y+1].z>0x100000)&&(rotated[x+1][y].z>0x100000)&&(rotated[x][y+1].z>0x100000))
            {

                drawing_mode(DRAW_MODE_TRANS,0,0,0);
                color_map=&tableGlenz8C;
                _triangle(lum,  fixtoi(projected[x  ][y  ].x),fixtoi(projected[x  ][y  ].y)
                          ,fixtoi(projected[x  ][y+1].x),fixtoi(projected[x  ][y+1].y)
                          ,fixtoi(projected[x+1][y  ].x),fixtoi(projected[x+1][y  ].y),
                          fixtoi(grille[x][y].z));
                _triangle(lum,  fixtoi(projected[x  ][y+1].x),fixtoi(projected[x  ][y+1].y)
                          ,fixtoi(projected[x+1][y+1].x),fixtoi(projected[x+1][y+1].y)
                          ,fixtoi(projected[x+1][y  ].x),fixtoi(projected[x+1][y  ].y),
                          fixtoi(grille[x][y].z));
            }

        }
}





void PartSalle(void)
{
    BITMAP *lum,*img,*small;
//  K3DTObject salle;
//  K3DTObject obj;
    int alpha;
    int drawSmall = 0;

    small = create_bitmap_ex(8,80,50);
    lum = create_bitmap_ex(8,320,200);
    img = create_bitmap_ex(8,320,200);

    K3DSetObjectRendering(&O3DTrois, TEXTURE, QSORT, Z);
    K3DSetObjectMap(&O3DTrois,data[Bmap8A].dat);
    set_projection_viewport(0,0,320,200);
    set_palette(pal);

    K3DPlaceObject(&O3DTrois,0,itofix(0),itofix(mouse_x*2),itofix(VBLframe),itofix(0),itofix(VBLframe));
    K3DEnvMapObject(&O3DTrois);
    K3DRotateObject(&O3DTrois);


    clear_to_color(lum,128);

    alpha=128;

    while (GetPosition() < 0x1400)
    {
        int x,y,i,z;


        z = 161+fixtoi(160*fcos(itofix(VBLframe)/2));

        clear_to_color(small,128);
//   clear_to_color(lum,128);

        if (alpha > -128 )
        {
            ThresholdSmoothMono(lum,data[BTransition1].dat, alpha , 100, 128 , 255);
            alpha-=2*VBLdelta;
        }
        else
        {
            drawSmall = 1;
            MotionBlurMono(lum,10,128);
        }

        DrawColorPlasma(img,VBLframe,2+z/10);

        set_projection_viewport(0,0,320,200);
        K3DSetObjectRendering(&O3DTrois, TEXTURE, QSORT, Z);
        K3DPlaceObject(&O3DTrois,0,itofix(0),itofix(200+z),itofix(VBLframe)/3,itofix(0),itofix(VBLframe)/2);
        K3DRotateObject(&O3DTrois);
        K3DEnvMapObject(&O3DTrois);
        for (i=0;i<O3DTrois.faces;i++)
        {
            if (polygon_z_normal((V3D*)&O3DTrois.rotated[O3DTrois.face[i].v1],
                                 (V3D*)&O3DTrois.rotated[O3DTrois.face[i].v2],
                                 (V3D*)&O3DTrois.rotated[O3DTrois.face[i].v3]) > 0 )
                O3DTrois.face[i].visible = 0;
        }
        K3DProjectObject(&O3DTrois);
        K3DDrawObject(img,&O3DTrois);

        if (drawSmall)
        {
            drawing_mode(DRAW_MODE_TRANS,0,0,0);
            color_map=&tableGlenz8D;
            set_projection_viewport(0,0,80,50);

            K3DSetObjectRendering(&O3DTrois, FLAT, NONE, Z);
            K3DPlaceObject(&O3DTrois,0,itofix(0),itofix(400),itofix(-VBLframe),itofix(VBLframe*3),itofix(VBLframe));
            K3DRotateObject(&O3DTrois);
            K3DProjectObject(&O3DTrois);
            K3DDrawObject(small,&O3DTrois);
            rect(small,0,0,79,49,192);

            for (y=0;y<4;y++)
                for (x=0;x<4;x++)
                {
//      if ( (rand()&3) >= 3)
                    if ( x == ((GetPosition()>>2)&3) )
                        blit(small,lum,0,0,x*80,y*50,80,50);
                    if ( y == ((GetPosition()>>2)&3) )
                        blit(small,lum,0,0,x*80,y*50,80,50);
                }

        }
        mergeBitmap(buffer,img,lum,tablePlasma);

        if ( (((GetPosition()+2)&4) != 0) && GetPosition()>=0x1300)
            draw_rle_sprite(buffer,data[BLinux].dat,0,0);

        Update(buffer);

        while (key[KEY_F1])
            {}
    }
}

void Part3D(void)
{
    BITMAP *lum,*map,*height,*img,*small;
    float intensity,speed;
    int level=0,i;

    img = create_bitmap_ex(16,320,200);
    small = create_bitmap_ex(8,80,50);
    lum = create_bitmap_ex(8,320,200);
    map = create_bitmap_ex(8,256,256);
    height = create_bitmap_ex(8,256,256);

    K3DSetObjectRendering(&O3DGisquet, TEXTURE, QSORT, Z);
    K3DSetObjectMap(&O3DGisquet,map);
    K3DSetObjectMap(&O3DGisquet,data[Bmap8A].dat);
    K3DSetObjectMap(&O3DAutre,data[Bmap8A].dat);
    set_projection_viewport(0,0,320,200);

    memcpy(pal,data[Pmap8A].dat,sizeof(PALETTE));
    set_palette(pal);

    K3DPlaceObject(&O3DGisquet,0,itofix(0),itofix(mouse_x*2),itofix(VBLframe),itofix(0),itofix(VBLframe));
    K3DRotateObject(&O3DGisquet);
    K3DEnvMapObject(&O3DGisquet);

    K3DPlaceObject(&O3DAutre,0,itofix(0),itofix(mouse_x*2),itofix(VBLframe),itofix(0),itofix(VBLframe));
    K3DRotateObject(&O3DAutre);
    K3DEnvMapObject(&O3DAutre);


    clear_to_color(lum,128);

    clear(buffer8);

    level=0;
    VBLdelta=0;
    WaitVBL();
    while (GetPosition() < 0x1F00)
    {
        int i,zx,zy;

        clear_to_color(lum,0);
//   blit(data[BMask].dat,lum,0,0,0,0,320,200);


        color_map = &tableGlenz8D;


        level += VBLdelta/2;

        zx = (level/2)+(level/2)*sin(VBLframe*2*3.14/100);
        zy = (level/2)+(level/2)*sin(VBLframe*2*3.14/76);

//   stretch_blit(data[Bmap8A].dat,buffer8,0,0,256,256,-zx/2,-zy/2,320+zx,200+zy);
        DeformBitmapScroll(buffer8,data[BPlasma2].dat,data[BPlasma].dat,
                           60+60*sin(VBLframe*3.14/500),60+60*sin(VBLframe*3.14/100),
                           60+60*sin(VBLframe*3.14/200),60+60*sin(VBLframe*3.14/300) );

        if ((GetPosition()&255) == 0x00)
            intensity = 20;
        if ((GetPosition()&255) == 0x20)
            intensity = 20;
        if ((GetPosition()&255) == 0x30)
            intensity = 20;
        intensity = intensity * 0.90;

        intensity = MID(0,intensity,50);

        if (GetPosition() < 0x1D00)
        {
            K3DPlaceObject(&O3DGisquet,0,itofix(0),itofix(200),40*fsin(itofix(VBLframe)),itofix(VBLframe)/2,itofix(0));
            K3DRotateObject(&O3DGisquet);

            for (i=0;i<O3DGisquet.vertexes;i++)
            {
                O3DGisquet.rotated[i].y += ftofix(intensity*cos((VBLframe*15+i*6)*2*3.1415927/300));
            }

            K3DBackFaceObject(&O3DGisquet);
            K3DSetObjectRendering(&O3DGisquet, TEXTURE, QSORT, Z);

            if ( ((GetPosition() & 0x0F ) == 0x04) ||
                    ((GetPosition() & 0x0F ) == 0x0A) ||
                    ((GetPosition() & 0x0F ) == 0x0C) ||
                    ((GetPosition() & 0x0F ) == 0x0F) )
                set_projection_viewport(-20,-20,360,240);
            else
                set_projection_viewport(0,0,320,200);

            K3DProjectObject(&O3DGisquet);
            K3DDrawObject(lum,&O3DGisquet);
        }
        else
        {
            K3DPlaceObject(&O3DAutre,0,itofix(0),itofix(200),40*fsin(itofix(VBLframe)),itofix(VBLframe)/2,itofix(VBLframe));
            K3DRotateObject(&O3DAutre);
            for (i=0;i<O3DAutre.vertexes;i++)
            {
                O3DAutre.rotated[i].y += ftofix(intensity*cos((VBLframe*15+i*6)*2*3.1415927/300));
            }

            K3DBackFaceObject(&O3DAutre);
            K3DSetObjectRendering(&O3DAutre, TEXTURE, QSORT, Z);

            if ( ((GetPosition() & 0x0F ) == 0x04) ||
                    ((GetPosition() & 0x0F ) == 0x0A) ||
                    ((GetPosition() & 0x0F ) == 0x0C) ||
                    ((GetPosition() & 0x0F ) == 0x0F) )
                set_projection_viewport(-20,-20,360,240);
            else
                set_projection_viewport(0,0,320,200);

            K3DProjectObject(&O3DAutre);
            K3DDrawObject(lum,&O3DAutre);
        }

        draw_sprite(lum,small,0,0);
        stretch_blit(lum,small,0,0,320,200,0,0,80,50);

        mergeBitmap(buffer,buffer8,lum,table3D);


        Update(buffer);


        while (key[KEY_F1])
            {}
    }


    clear_to_color(lum,128);
    blit(buffer,img,0,0,0,0,320,200);
    {
        BITMAP *grid[4][4];
        int i,j;
        float pos=80,increment=1;

        for (i=0;i<4;i++)
            for (j=0;j<4;j++)
                grid[i][j] = create_sub_bitmap(img,i*80,j*50,80,50);

        while (pos<400)
        {
            Update(buffer);

            DrawSimpleTunnel(buffer8, lookup2,
                             fixtoi(140*fsin(itofix(VBLframe)/4)),
                             fixtoi(80*fcos(itofix(VBLframe)/3)),
                             VBLframe*0.8, VBLframe*0.5, data[Bmap8A].dat);

            mergeBitmap(buffer,buffer8,lum,tablePlasmaBump);


            for (i=0;i<4;i++)
                for (j=0;j<4;j++)
                {
                    double x,y,z,z2;

                    x = 40+80*(i-2.0);
                    y = 25+50*(j-2.0);

                    z = pos;
                    z = MID(80,z,10000);

                    z2 = (z-80)/2;
                    x = (((double)x) * z)/80.0;
                    y = (((double)y) * z)/80.0;
                    blit(grid[i][j],buffer,0,0,120+x,75+y,80,50);
                }
            pos += increment*VBLdelta;
            increment *= 1.05;
        }

    }

    destroy_bitmap(lum);
    destroy_bitmap(img);
    destroy_bitmap(map);
    destroy_bitmap(small);
    destroy_bitmap(height);

}


// ###############################################################
// End Scroll

COLOR_MAP tableGlenzScroll;
void PartScroll(void)
{
    BITMAP * dirty;
    int pos=0,oldPos=0;
    int x,y;

    dirty = create_bitmap_ex(8,640,480);
    set_palette(black_palette);
    blit(data[BBump].dat,screen,0,0,0,0,640,480);

//  MIDASsetPosition(play,0x24);
#ifdef USE_JGMOD
    mi.trk=0x24;
#endif

    for (x=0;x<63;x++)
    {
        vsync();
        vsync();
        fade_interpolate(black_palette,palLogo,pal,x,0,255);
        set_palette(pal);
#ifdef USE_JGMOD
        set_mod_volume(x*2);
#endif
    }
    blit(data[BBump].dat,screen,0,0,0,0,640,480);
    finalScrollPosition = 0;
    while ((pos> -1580 ) && !key[KEY_ESC])
    {

#ifdef USE_JGMOD
        mi.trk=0x24;
#endif


        pos-=1;
        x = fixtoi(8*fcos(itofix(pos*2)));
        y = fixtoi(8*fsin(itofix(pos*2.2)));
        blit(data[BBump].dat,dirty,0,0,0,0,640,480);
        draw_rle_sprite(dirty,data[BScrollOmbre].dat,(640-256)/2+x,pos+480+y);
        draw_rle_sprite(dirty,data[BScroll].dat,(640-256)/2,pos+480);
        vsync();
        blit(dirty,screen,0,0,0,0,640,480);
    }

    for (x=64;x>=0;x--)
    {
        vsync();
        fade_interpolate(black_palette,palLogo,pal,x,0,255);
        set_palette(pal);
#ifdef USE_JGMOD
        set_mod_volume(x*2);
#endif
    }
}

typedef struct TMeteorPos
{
    int x,y,z;
} TMeteorPos;


void PartGreets(void)
{
    struct  K3DParticuleSystem ps;
    K3DTObject meteor;
    BITMAP   * map,*lum;
#define    MAX_METEOR 35
#define    MAX_STAR   300
    TMeteorPos pos[MAX_METEOR];
    int i,n;

    BITMAP * greets;

    greets = load_bitmap("gfx/calodox.pcx",pal);


    map = data[Bmap8A].dat;

//  set_palette(palGreets);
    set_palette(pal);

    lum = create_bitmap_ex(8,320,200);

    K3DInitObject(&meteor);
    K3DLoadObjectASC("patate.asc",&meteor);
    K3DSetObjectRendering(&meteor, TEXTURE, QSORT, Z);
    K3DSetObjectMap(&meteor,map);

    set_projection_viewport(0,0,320,200);
    meteor.zmax=27;
    meteor.clipping=80;

    K3DPlaceObject(&meteor,0,itofix(0),itofix(200),itofix(-100),itofix(0),itofix(0));
    K3DRotateObject(&meteor);
    K3DEnvMapObject(&meteor);

    for (n=0;n<MAX_METEOR;n++)
    {
        pos[n].x = (rand()%4000)-2000;
        pos[n].y = 500;  //(rand()%2000)-1000;
        pos[n].z = (rand()%5000);
    }

    K3DParticuleInit(&ps,MAX_STAR);
    for (i=0;i<MAX_STAR;i++)
    {
        ps.particule[i].x   = itofix(1000-rand()%2000);
        ps.particule[i].y   = itofix(1000-rand()%2000);
        ps.particule[i].z   = itofix(1000-rand()%2000);
        ps.particule[i].lum = i;
    }
    set_projection_viewport(0,0,320,200);



    while (!key[KEY_ESC])
    {

        WaitVBL();
        mergeBitmap(buffer,buffer8,lum,tableTunnel);
        blit(buffer,screen,0,0,0,10,320,200);
        MotionBlurMono(buffer8,5,0);
        clear_to_color(lum,128);
//   putpixel(buffer8,mouse_x,mouse_y,255);


        for (i=0;i<MAX_STAR;i++)
            ps.particule[i].z = (ps.particule[i].z-itofix(VBLdelta*2))&itofix(2047);

        K3DParticuleRotate(&ps);
        K3DParticuleProject(&ps);
        K3DParticulePixel(buffer8,&ps);


        for (n=0;n<MAX_METEOR;n++)
        {
            pos[n].z = (pos[n].z-((int)VBLdelta)*90) & 4095;

            K3DPlaceObject(&meteor,itofix(pos[n].x),itofix(pos[n].y),itofix(200+pos[n].z),itofix(VBLframe*3+n),itofix(n*19),itofix(VBLframe+n*8));
            K3DRotateObject(&meteor);
            K3DProjectObject(&meteor);
            K3DDrawObject(buffer8,&meteor);

        }

//   for(i=1;i<5;i++)
        {
            int z,y;

            if (mouse_b)
                z = 0;

            z +=VBLdelta;
            y = z;

            if (z>200)
            {
                z +=VBLdelta;
                y = (400-z);
            }


            stretch_sprite(buffer8,greets,160-(z)/2,100-(y)/2,z,y);
        }
    }

    destroy_bitmap(greets);
    K3DDeleteObject(&meteor);
}

void testTunnel(void)
{
    BITMAP * map;
    int pos;

    map = create_bitmap_ex(8,256,256);

    set_palette(pal);

    color_map = &tableGlenz8B;
    while (!key[KEY_ESC])
    {

        pos += 2;

        WaitVBL();
        blit(buffer8,screen,0,0,0,10,320,200);
        blit(data[Bmap8A].dat,map,0,0,0,0,256,256);
        draw_trans_sprite(map,data[Bmap8A/*Star*/].dat,0,(pos&511)-255);
        draw_trans_sprite(map,data[Bmap8A/*Star*/].dat,0,((pos+256)&511)-255);
        blit(map,screen,0,0,320,0,256,256);
        DrawSimpleTunnel(buffer8, lookup1, 0, 0, -VBLframe, VBLframe*0, map);
    }

}


// ###############################################################
//
void PartPlasma(void)
{
    BITMAP *lum,*img;

    lum = create_bitmap_ex(8,320,200);
    img = create_bitmap_ex(8,320,200);

    while (!key[KEY_ESC])
    {


        Update(buffer);
        DrawPlasma(img,data[BPlasma].dat,VBLframe,mouse_y,mouse_b);
        DrawColorPlasma(lum,VBLframe,(float)mouse_x/10.0);
        mergeBitmap(buffer,img,lum,tablePlasma);
    }
}

// ###############################################################
//

void drawEclair(BITMAP * bmp, int x1, int x2, int y, int div)
{
    int i,x,ya,yb,p,intensity;

    p = (x2-x1) / (div+1);
    intensity = 20;


    drawing_mode(DRAW_MODE_TRANS,0,0,0);
    set_trans_blender(0,0,0,128);
    x = x1;
    yb = y+(rand()%(intensity*2))-(intensity);

    line(bmp, x1, y  , x+p-1, yb   ,makecol16(255,255,255));
    line(bmp, x1, y-1, x+p-1, yb-1 ,makecol16(128,128,255));
    line(bmp, x1, y+1, x+p-1, yb+1 ,makecol16(128,128,255));

    for (i=1;i<div;i++)
    {
        x += p;
        ya = yb;
        yb = y+(rand()%(intensity*2))-(intensity);

        line(bmp,x ,ya-1 ,x+p-1 ,yb-1 ,makecol16(128,128,255));
        line(bmp,x ,ya+1 ,x+p-1 ,yb+1 ,makecol16(128,128,255));
        line(bmp,x ,ya   ,x+p-1 ,yb   ,makecol16(255,255,255));
    }
    x += p;
    line(bmp,x ,yb   ,x2 ,y   ,makecol16(255,255,255));
    line(bmp,x ,yb-1 ,x2 ,y-1 ,makecol16(128,128,255));
    line(bmp,x ,yb+1 ,x2 ,y+1 ,makecol16(128,128,255));



}


unsigned short tablePower[64*64];


void PartGuru(void)
{
    BITMAP *temp;
    RLE_SPRITE *guru;
    BITMAP *lum,*img;
    BITMAP *power;
    BITMAP *feu,*small1,*small2;
    PALETTE palFeu;

    int fEclair=0;
    int fTamTam=1;
    int fSmall =1;
    int fWaves =0;
    int clearColor=128;

    int   tamPower=255;
    float eclairPower=0.0;
    int   alpha = 255;

    memcpy(palFeu,data[PFeu].dat,sizeof(PALETTE));


    // Bitmaps
    lum = create_bitmap_ex(8,320,200);
    img = create_bitmap_ex(8,320,200);

//  guru = data[Bguru].dat;
    small1 = create_bitmap_ex(8,80,50);
    small2 = create_bitmap_ex(8,80,50);
    feu = create_bitmap_ex(8,64,64);
    power = create_bitmap_ex(16,64,64);
    temp = create_bitmap_ex(16,64,64);
    set_palette(palFeu);
    guru = data[Bguru].dat;

    while ( GetPosition()< 0x1A00 )
    {
        int i;
        fixed z;
        fixed a;

        z = ftofix(0.75)+fcos(itofix(VBLframe/3))/4;
        a = 255*fcos(itofix(VBLframe)/4);

        Update(buffer);

        // control

        if ( (GetPosition()>= 0x1500))
            fEclair = 1;

//    if ( (GetPosition()>= 0x1800))
//      fSmall  = 0;

        if ( (GetPosition()>= 0x1800))
        {
            fTamTam = 0;
            fWaves  = 1;
            fEclair = 0;
        }

        clear(buffer);

        if ( (GetPosition() >> 8 == 0x17)  && (GetPosition()&1) )
            clearColor = 128+2*(GetPosition()&255);
        else
            clearColor = 128;

        clear_to_color(lum,clearColor);


        if (fTamTam)
        {
            tamPower -= 10*VBLdelta;
            tamPower = MID(0,tamPower,255);

            if ( ((GetPosition() & 0x0F ) == 0x04) ||
                    ((GetPosition() & 0x0F ) == 0x0A) ||
                    ((GetPosition() & 0x0F ) == 0x0C) ||
                    ((GetPosition() & 0x0F ) == 0x0F) )
                tamPower = 255;
            DrawPlasma(img,data[BPlasma].dat,VBLframe,tamPower,0);
        }

        if (fEclair)
        {
            float delta;

            eclairPower += ((float )VBLdelta)*delta;
            eclairPower = MID(0.0,eclairPower,3.0);

            if ( (((GetPosition() & 0xFF ) >= 0x00) && ((GetPosition() & 0xFF ) <= 0x04)) ||
                    (((GetPosition() & 0xFF ) >= 0x20) && ((GetPosition() & 0xFF ) <= 0x24)) ||
                    (((GetPosition() & 0xFF ) >= 0x30) && ((GetPosition() & 0xFF ) <= 0x34))  )
                delta = 0.5;
            else
                delta = -0.03;
        }

        if (fSmall)
        {
            drawing_mode(DRAW_MODE_TRANS,0,0,0);
            color_map=&tableGlenz8D;
            set_projection_viewport(0,0,80,50);

            clear_to_color(small2,clearColor);
            K3DSetObjectRendering(&O3DSalle, FLAT, NONE, Z);
            K3DPlaceObject(&O3DSalle, 0, itofix(50), itofix(150)+50*fsin(VBLframe<<14), itofix(40)+60*fcos(2*VBLframe<<14), 128<<16, VBLframe<<16);
            K3DRotateObject(&O3DSalle);
            K3DProjectObject(&O3DSalle);
            K3DDrawObject(small2,&O3DSalle);
            rect(small2,0,0,79,49,192);

            if (GetPosition() < 0x1800)
            {
                clear_to_color(small1,clearColor);
                K3DSetObjectRendering(&O3DCity, FLAT, NONE, Z);
                K3DPlaceObject(&O3DCity, 0, itofix(100), itofix(200)+100*fsin(2*VBLframe<<16), itofix(40)+20*fcos(2*VBLframe<<14), 128<<16, 2*VBLframe<<16);
                K3DRotateObject(&O3DCity);
                K3DProjectObject(&O3DCity);
                K3DDrawObject(small1,&O3DCity);
                rect(small1,0,0,79,49,192);

                if (GetPosition() >= 0x1600)
                    blit(small1,lum,0,0,40,25,80,50);
                if (GetPosition() >= 0x1620)
                    blit(small2,lum,0,0,200,25,80,50);
                if (GetPosition() >= 0x1630)
                    blit(small1,lum,0,0,200,125,80,50);
                if (GetPosition() >= 0x1700)
                    blit(small2,lum,0,0,40,125,80,50);
            }
            else
            {
                blit(small2,lum,0,0,0,0,80,50);
                blit(small2,lum,0,0,0,50,80,50);
                blit(small2,lum,0,0,240,100,80,50);
                blit(small2,lum,0,0,240,150,80,50);
            }
        }

        if (fWaves)
        {
            set_projection_viewport(0,0,320,200);
            Draw3DWaves(lum, itofix(300),
                        itofix(160)+30*fsin(itofix(VBLframe)/4),0,itofix(VBLframe)/2);
        }

        mergeBitmap(buffer,img,lum,tableGuru);
        if ( (GetPosition()<= 0x1720))
        {
            draw_rle_sprite(buffer,guru,0,0);
            clear(power);
            {
                fixed z;
                fixed a;

                z = ftofix(0.75)+fcos(itofix(VBLframe/3))/4;
                a = 255*fcos(itofix(VBLframe)/4);
                rotate_scaled_sprite(power,data[Bpower].dat,32-(32*fixtof(z)),32-(32*fixtof(z)),a,z);
                z = ftofix(0.75)+fcos(itofix(VBLframe/2))/4;
                a = 255*fcos(itofix(VBLframe+50)/4);
                rotate_scaled_sprite(temp,data[Bpower].dat,32-(32*fixtof(z)),32-(32*fixtof(z)),a,z);
            }
            set_trans_blender(0,0,0,mouse_y);


            drawLight( buffer, buffer, power,  57, 49);
            drawLight( buffer, buffer, power, 200, 49);
        }
        else
            if ( (GetPosition()<= 0x1800))
            {
                set_trans_blender(0,0,0,alpha);
                draw_trans_rle_sprite(buffer,guru,0,0);
                alpha-=2*VBLdelta;
                alpha = MID(0,alpha,255);
            }


        if (fEclair)
        {
            int eclairPowerC;

            if (GetPosition() & 1)
            {
                eclairPowerC = eclairPower/2.0;
                drawLight( buffer, buffer, data[BHalo1].dat, 200, 49);
                drawLight( buffer, buffer, data[BHalo1].dat, 57, 49);
            }
            else
                eclairPowerC = eclairPower;

            for (i=1;i<eclairPowerC;i++)
            {
                drawEclair(buffer,89,232,81,10);
            }

        } // draw eclair


        while (key[KEY_F1])
            {}

    }
    destroy_bitmap(power);
    destroy_bitmap(temp);
    destroy_bitmap(feu);
    destroy_bitmap(small1);
    destroy_bitmap(small2);
    destroy_rle_sprite(guru);
}

struct TBumpTable plasmaBump;

void PartPlasmaBump(void)
{
    BITMAP * map;
    BITMAP * lum;
    int pos;

    map = create_bitmap_ex(8,256,256);
    lum = create_bitmap_ex(8,320,200);
    clear_to_color(lum,128);
    memcpy(pal,data[Pmap8A].dat,sizeof(PALETTE));
    set_palette(pal);


    while (GetPosition() <= 0x2300)
    {
        int i;

        color_map = &tableGlenz8B;

        pos = 2*VBLframe;

        Update(buffer);
        blit(data[Bmap8A].dat,map,0,0,0,0,256,256);

        if (GetPosition() >= 0x1F20)
        {
            draw_trans_sprite(map,data[Bmap8A].dat,0,(pos&511)-255);
            draw_trans_sprite(map,data[Bmap8A].dat,0,((pos+256)&511)-255);
        }

        if ((GetPosition() >= 0x1F30))
        {
            draw_trans_rle_sprite(map,data[BStars].dat,(pos&511)-255,0);
            draw_trans_rle_sprite(map,data[BStars].dat,((pos+256)&511)-255,0);
        }
        DrawSimpleTunnel(buffer8, lookup2,
                         fixtoi(140*fsin(itofix(VBLframe)/4)),
                         fixtoi(80*fcos(itofix(VBLframe)/3)),
                         VBLframe*0.8, VBLframe*0.5, map);

        if (GetPosition() >= 0x2000)
        {
            clear_to_color(lum,128);
            clear_to_color(plasmaBump.lightMap,128);
            for (i=0;i<10;i++)
            {
                int x,y,d;

                d = i*117;

                // draw the wonderful spots
                x = 24
                    *(cos((VBLframe+d)*3.14*2/200)
                      +cos((VBLframe+d)*3.14*2/120)
                      +cos((VBLframe+d)*3.14*2/100)
                      +cos((VBLframe+d)*3.14*2/2800));

                y = 24
                    *(sin((VBLframe+d)*3.14*2/200)
                      +sin((VBLframe+d)*3.14*2/1400)
                      +sin((VBLframe+d)*3.14*2/140)
                      +sin((VBLframe+d)*3.14*2/400));
                draw_trans_sprite(plasmaBump.lightMap,data[BHalo].dat,128+y-32,128+x-32);
            }

            DrawBump(lum, 160+fixtoi(100*fsin(itofix(VBLframe))), 100, &plasmaBump);
            draw_trans_sprite(lum,data[BTransition].dat,0,0);
        }

//    DrawColorPlasma(lum, VBLframe, mouse_x/10.0);
//    ThresholdSmoothMono(lum,data[BRmxStyler].dat,(int)(mouse_x)-160,mouse_y/4,128,255);

        if (GetPosition() >= 0x2300)
            clear_to_color(lum,0);
        if ((GetPosition() > 0x2230) && (GetPosition()&1))
            clear_to_color(lum,255);

        mergeBitmap(buffer,buffer8,lum,tablePlasmaBump);
    }
    destroy_bitmap(map);
    destroy_bitmap(lum);
}

void testPlaques(void)
{
}

// ###############################################################
//
void Styler2Precalk(void)
{
    puts("  Styler 2 is being processed.");
    printf("  ");
    PrecalkOmbre();
    printf(".");
    fflush(stdout);
    PrecalkLumiereParticule();
    printf(".");
    fflush(stdout);
    PrecalkTunnel();
    printf(".");
    fflush(stdout);
    PrecalkBump();
    printf(".");
    fflush(stdout);
    InitBumpTable(&plasmaBump, 0);
    SetBumpTable(&plasmaBump, data[BTransition].dat);
}

// ###############################################################
//
void Demo(void)
{
    set_color_depth(16);
    /*switch(UpdateMode)
    {
      case UM_WIN:
          set_gfx_mode(GFX_AUTODETECT_WINDOWED,320,200,0,0);
        break;
      case UM_WIN2x:
          set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,400,0,0);
        break;
      case UM_FULLSCREEN:
          set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,320,200,0,0);
        break;
      case UM_FULLSCREEN240:
      case UM_FULLSCREEN240b:
          set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,320,240,0,0);
        break;
      case UM_FULLSCREEN480:
      case UM_FULLSCREEN480b:
          set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,640,480,0,0);
        break;
    }*/
    
    SCALE_X = SCREEN_W/320.0;
    SCALE_Y = SCREEN_H/200.0;        
    
    clear(screen);
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    buffer8 = create_bitmap_ex(8, SCREEN_W, SCREEN_H);

	_image = malloc(SCREEN_W*SCREEN_H);


    /* load the datafile into memory */
    set_color_conversion(COLORCONV_NONE);
    data = load_datafile("styler2.dat");
    if (!data)
    {
        allegro_exit();
        printf("  Error loading styler2.dat!\n\n");
        exit(201);
    }

    Styler2Precalk();

    PartOmbre();

    PartLumiereParticule();
    PartTunnel();
    PartBump();

    PartParticules();
    PartSalle();

//  MIDASsetPosition(play,0x14);
    PartGuru();


//  MIDASsetPosition(play,0x1A);
//  mi.trk=0x1d;
    Part3D();

//  MIDASsetPosition(play,0x1F);
    PartPlasmaBump();




    destroy_bitmap(buffer);
    destroy_bitmap(buffer8);

    K3DDeleteObject(&O3DAutre);
    K3DDeleteObject(&O3DGisquet);
    K3DDeleteObject(&O3DTrois);
    K3DDeleteObject(&O3DCity);
    K3DDeleteObject(&O3DSalle);

}

void FatalError(char * error)
{
    allegro_exit();
    printf("  Error : %s !\n\n",error);
    exit(1);
}


int main(int argc, char ** argv)
{
    int RR320x200x16=70000;
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    allegro_init();

    printf("Styler 2 (c) KNIGHTS 1998\n  FINAL Version\n");
    printf("  ( Run with any parameter to run sound setup )\n");

    puts("  Loading.");

    if (argc>1)
    {
        if (strcasecmp(argv[1],"2x")==0) UpdateMode=UM_WIN2x;
        if (strcasecmp(argv[1],"200")==0) UpdateMode=UM_FULLSCREEN;
        if (strcasecmp(argv[1],"240")==0) UpdateMode=UM_FULLSCREEN240;
        if (strcasecmp(argv[1],"240b")==0) UpdateMode=UM_FULLSCREEN240b;
        if (strcasecmp(argv[1],"480")==0) UpdateMode=UM_FULLSCREEN480;
        if (strcasecmp(argv[1],"480b")==0) UpdateMode=UM_FULLSCREEN480b;
    }


    //install_timer ();
    install_keyboard ();
    set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 400, 0, 0);


    MusicStart("styler2.xm");
    SDL_AddTimer(50, MusicUpdate, 0);
    SDL_AddTimer(1000,TimerHandler, 0);

    Demo();

    MusicStop();

    set_color_depth(8);

    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);

    /*  switch(UpdateMode)
      {
        case UM_WIN:
        case UM_WIN2x:
            set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
          break;
        case UM_FULLSCREEN:
        case UM_FULLSCREEN240:
        case UM_FULLSCREEN240b:
        case UM_FULLSCREEN480:
        case UM_FULLSCREEN480b:
            set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,640,480,0,0);
          break;
      }
    */

    PartScroll();


    /* unload the datafile when we are finished with it */
    unload_datafile(data);

    set_gfx_mode(GFX_TEXT, 640, 480, 0, 0);
    printf("\n                       Styler 2 (c) KNIGHTS 1998  [FINAL]\n\n");
    puts("   _____ _/\\_______/\\________________/\\______ _ ___/\\ _ __________________"     );
    puts("\\\\ \\   _/   \\ ___    \\________/        )  __/____    \\______   __________/ //"  );
    puts("   /  ______/  \\      \\      /  ______/   __     \\     ____/_______      \\   "  );
    puts("  /   \\_    \\_  \\      \\    /   \\_    \\_   \\      \\    \\      \\    \\  sns \\   " );
    puts(" (______\\    /___\\______)__(______\\    /____\\______)___________)___________).  ");
    puts("         \\  /                      \\  /                    :    knights!    :..");
    puts("          \\/                        \\/                     :................: :");
    puts("                                                             :................:      ");
    puts("            .featuring.                 .Special Guests.");
    puts("     Light-Speed Code : Impulse         MIDAS    : The best Music lib.");
    puts("     Wonderfull Music : Logic Dream     Allegro  : The best Gfx lib.");
    puts("       Sweet Graphics : Tigrou          DJGPP    : The best Compiler.");
    puts("Eye-Catching Graphics : Stuff           Delphine : Ideas & support.\n");
    puts("\n             If you are a talented 3D artist, contact us...");
    puts("\n                      http://Impulse.CiteWeb.net");

    return 0;
}
END_OF_MAIN()

