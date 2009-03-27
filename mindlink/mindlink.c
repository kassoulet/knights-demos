/*

  MindLink

  Coded by Gautier "Impulse" PORTET

  09/97 - 12/97

  (c) 1997-2002 KNIGHTS

  not so fast code
  ugly design
  buggy module player
  but here is it: win32/linux versions

  updated in 2009 !
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>
#include "zlib.h"
#include <allegro.h>
#include "data.h"

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT= 800;



extern int MusicGetPosition(void);
extern void MusicStart(char *filename);
extern void MusicStop();
extern int MusicUpdate();
extern void MusicSetPosition(int position);
extern void MusicDump(char *filename);

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

gzFile zDump;
Uint32 dump_active=0;
int dump_frame=0;
int dump_fps=30;

BITMAP *screen_copy=NULL;
PALETTE pal_copy;
gzFile dumpfile;

void _DumpFrame()
{
    char filename[1024];
    char cmd[1024];
    
    
    sprintf(filename, "dump/%08d.tga", dump_frame);
    dump_frame++;
    
    save_bitmap(filename, screen_copy, pal_copy);
    
    //sprintf(cmd, "convert -scale 50%% dump/%08d.tga dump/%08d.png ; rm dump/%08d.tga &", dump_frame, dump_frame, dump_frame);
    //system(cmd);
    
    
    //fwrite(buffer->dat,  SCREEN_W*SCREEN_H, 1, dumpfile);
    /*if (buffer && buffer->dat) {
        gzwrite(dumpfile, pal, sizeof(pal));
        gzwrite(dumpfile, buffer->dat, SCREEN_W*SCREEN_H);
    }*/
}

Uint32 _old;

#define DUMP_DELAY 20

//Uint32 DumpFrame(Uint32 interval, void *param)
void DumpFrame()
{
    Uint32 diff, ticks;
    ticks = SDL_GetTicks();
    diff = (ticks - _old);
    
    if (diff < DUMP_DELAY) {
        return;
        //printf("frame skip #%d (%dms)\n", dump_frame, diff);
    }
    _old = ticks;
    blit(screen, screen_copy, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    get_palette(pal_copy);
    SDL_CreateThread(_DumpFrame, 0);
    //_DumpFrame();
    //return interval;
}

/*
void DumpFrameThread()
{
    char filename[1024];
    Uint32 ticks, old=0;
    int diff;
    PALETTE pal;
    
    while (dump_active) {
    
        do {
            ticks = SDL_GetTicks();
            diff = ticks-old;
            if (diff > DUMP_DELAY)
                printf("skip frame %d (%d)\n", dump_frame, diff);
            SDL_Delay(0);
        } while (diff < DUMP_DELAY-1);     
        
        get_palette(pal);
        sprintf(filename, "dump/%08d.tga", dump_frame);
        save_bitmap(filename, buffer, pal);
        dump_frame++;
        old = ticks;
    }
}
*/

void DumpStart()
{
    mkdir("dump", 0777);
    dump_active = 1;
    //SDL_CreateThread(DumpFrameThread, 0);
    //dumpfile = gzopen("dump/dump.gz", "wb3");

    screen_copy = create_bitmap(SCREEN_W, SCREEN_H);

    //SDL_AddTimer(DUMP_DELAY-10, DumpFrame, 0);
}

void DumpEnd()
{
    printf("%d frames saved!\n", dump_frame);
    dump_active = 0;
    //fclose(dumpfile);
    //gzclose(dumpfile);
}


unsigned short *TunnelLookup;
unsigned short *TunnelLookup2;
unsigned short *PlasmaLookup;
unsigned short *PlasmaLookup2;


unsigned char *_image;
unsigned char _map[256*256];



int count;

int VBLframe=0,VBLfps=100,VBLcount=0,VBLold=0;
int VBLdelta=0;
double fVBLdelta=0;
int refreshRate;
int finalScrollPosition;


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
    VBLdelta = delta;
    fVBLdelta= delta;
    oldTicks = ticks;
}

void WaitVBL()
{
    UpdateTimer();

    DumpFrame();

    //if (key[KEY_ESC])
    //    exit(2);

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


void Update()
{
    WaitVBL();
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

PALETTE *palWater;
COLOR_MAP *tableWater,*tableGlenz;

RLE_SPRITE *text[11];


void Fatal(char* str)
{
    printf("Fatal : %s\n", str);
    exit(0);
}

/* all possible methods to draw a face */
enum K3DTfilling
{
    PLOT=0,
    WIREFRAME,
    FLAT,
    GOURAUD,
    GOURAUD_RGB,
    TEXTURE,
    TEXTURE_MASK,
    TEXTURE_SHADED,
    TEXTURE_CORRECTED,
    TEXTURE_MASK_CORRECTED,
    TEXTURE_SHADED_CORRECTED
};

/* type of sorting - bytesort is the fastest.*/
enum K3DTsorting
{
    NONE=0,
    QSORT,
    BYTESORT,
    FORCE_BACKFACE=128
};

/* the method used to compute faces light */
enum K3DTcolorModel
{
//  NONE=0,
    Z=1,
    NORMAL
};


typedef struct K3DTRendering
{
    enum K3DTfilling filling;
    enum K3DTsorting sorting;
    enum K3DTcolorModel  colorModel;
} K3DTRendering;


/* a 3D point / a vector ( u & v : for the texture mapping, c: color )  */
typedef struct K3DTVertex
{
    fixed x, y, z;
    fixed u,v;
    int color;
} K3DTVertex;

/* a face [WARNING K3D can ONLY HANDLE TRIANGLES !!]*/
typedef struct K3DTFace
{
    int v1, v2, v3;
    int color;
    BITMAP * map;
    char visible;
    int z; // *****
} K3DTFace;

/* the positions and rotations ( used as the place of an object or its speed )*/
typedef struct K3DTPlace
{
    fixed x, y, z;
    fixed rx, ry, rz;
} K3DTPlace;

/* an object : 2 arrays with vertexes & faces + its location & orientation */
typedef struct K3DTObject
{
    int                         vertexes,faces;
    struct K3DTVertex          *vertex;
    struct K3DTVertex          *rotated;
    struct K3DTVertex          *projected;
    struct K3DTFace            *face;
    struct K3DTPlace            place;
    struct K3DTRendering        rendering;
    fixed                       scale;
    fixed                       clipping;
} K3DTObject;

/* an array of objects */
typedef struct K3DTWorld
{
    int objects;
    struct K3DTObject *object;
    struct K3DTPlace place;
} K3DTWorld;

#define MAX_FACES 1024
unsigned char faceOrder[MAX_FACES],faceDepth[MAX_FACES];


void K3DInit(void)
{
}

void K3DInitObject(struct K3DTObject * obj)
{
    obj->scale = itofix(1);
    obj->vertexes = 0;
    obj->faces = 0;
    obj->vertex = NULL;
    obj->face = NULL;
    obj->clipping = itofix(5);
    obj->rendering.sorting = NONE;
    obj->rendering.colorModel = NORMAL;
    obj->rendering.filling = FLAT;

}

void K3DInitWorld(struct K3DTWorld * obj)
{
    obj->objects = 0;
    obj->object = NULL;
}

void K3DDeleteObject(struct K3DTObject * obj)
{
    if (obj->vertex != NULL)
        free(obj->vertex);
    if (obj->face != NULL)
        free(obj->face);
}

void K3DDeleteWorld(struct K3DTWorld * obj)
{
    int i;

    if (obj->object != NULL)
        for (i = 0;i < obj->objects;i++)
            K3DDeleteObject(&obj->object[i]);
}

/* internal helper : load a file line */
int LoadLine(FILE * f, char * line)
{
    char c;
    int i = 0;

    line[0] = 0;
    do
    {
        c = fgetc(f);
        if (c == EOF)
            return 0;
        if (c == '\n')
            return 1;
        line[i++] = c;
        line[i] = 0;
    }
    while (1);
}

int K3DLoadObjectASC(char * file, struct K3DTObject * obj)
{

    FILE * f;
    int i,fin;
    char line[250],temp[255];
    int faces,vertexes;

    line[0] = 0;
    f = fopen(file, "rb");
    if (f == NULL) return 0;
    do
    {
        /* until a tri-mesh found */
        do
        {
            /* search for all objects */
            LoadLine(f, line);
        }
        while (strncmp(line, "Named object:", 13) != 0);
        LoadLine(f, line);
    }
    while (strncmp(line, "Tri-mesh", 8) != 0);

    /* Tri-mesh, Vertices: 410     Faces: 718 */
    /*                   ^ here we go  */
    strcpy(line, strchr(line, ':')+2);
    strncpy(temp, line, strchr(line, 'F')-line);
    /*410     Faces: 718 */
    /*<------> get this  */
    temp[strchr(line, 'F')-line] = 0;
    vertexes = atoi(temp);

    strcpy(line, strchr(line, ':')+2);
    faces = atoi(line);

    obj->vertex = (struct K3DTVertex*) malloc(vertexes*sizeof(struct K3DTVertex));
    obj->rotated = (struct K3DTVertex*) malloc(vertexes*sizeof(struct K3DTVertex));
    obj->projected = (struct K3DTVertex*) malloc(vertexes*sizeof(struct K3DTVertex));
    obj->face = (struct K3DTFace*) malloc(faces*sizeof(struct K3DTFace));

    LoadLine(f, line);
    i = 0;
    fin = 0;
    do
    {
        /* load all vertexes */
        LoadLine(f, line);
        //      printf("\r%s",line);
        if (strncmp(line, "Face list:", 10) != 0)
        {
            /* convert vertex */

            if ((strstr(line, "X:")) != NULL)
            {
                /* Get the x,y,z vertexes */
                strcpy(line, strstr(line, "X:")+3);
                strcpy(temp, line);
                temp[strstr(line, "Y:")-line] = 0;
                obj->vertex[i].x = itofix(atoi(temp));

                strcpy(line, strstr(line, "Y:")+3);
                strcpy(temp, line);
                temp[strstr(line, "Z:")-line] = 0;
                obj->vertex[i].y = itofix(atoi(temp));

                strcpy(line, strstr(line, "Z:")+3);
                strcpy(temp, line);
                obj->vertex[i].z = itofix(atoi(temp));
                i++;

            }
            else
            {
            }

        }
        else
            fin = 1;

    }
    while (!fin);

    if (i != vertexes)
    {
        printf("Error while parsing vertexes (%d found,%d expexted)", i, vertexes);
        exit(1);
    }

    for (i = 0;i < faces;i++)
    {
        do
        {
            if (!LoadLine(f, line))
            {
                printf("Unexpected EOF\n");
                exit(2);
            }
        }
        while ((strstr(line, "Face")) == NULL);
        /*Face 717:    A:407 B:236 C:231 AB:1 BC:1 CA:1*/

        /* Get the face's vertexes */
        strcpy(line, strstr(line, "A:")+2);
        strcpy(temp, line);
        obj->face[i].v1 = atoi(temp);

        strcpy(line, strstr(line, "B:")+2);
        strcpy(temp, line);
        obj->face[i].v2 = atoi(temp);

        strcpy(line, strstr(line, "C:")+2);
        strcpy(temp, line);
        obj->face[i].v3 = atoi(temp);

    }

    if (faces >= MAX_FACES)
    {
        printf("error: please raise MAX_FACES to %d\n", faces);
        exit(1);
    }
    obj->vertexes = vertexes;
    obj->faces = faces;

    fclose(f);
    return 1;
}

void K3DSetObjectRendering(struct K3DTObject *obj, int f, int s, int c)
{
    obj->rendering.filling = f;
    obj->rendering.sorting = s;
    obj->rendering.colorModel = c;
}

void K3DSetObjectMap(struct K3DTObject *obj, BITMAP* map)
{
    int i;

    for (i = 0;i < obj->faces;i++)
        obj->face[i].map = map;
}

void K3DEnvMapObject(struct K3DTObject *obj)
{
    int i;

    //  if(obj->rendering.colorModel==Z)
    {
        for (i = 0;i < obj->vertexes;i++)
        {
            obj->projected[i].u = obj->rotated[i].x-obj->rotated[i].z;
            obj->projected[i].v = obj->rotated[i].y-obj->rotated[i].z;
        }
    }
}

void K3DPlaceObject(struct K3DTObject *obj, fixed x, fixed y, fixed z, fixed rx, fixed ry, fixed rz)
{
    obj->place.x = x;
    obj->place.y = y;
    obj->place.z = z;
    obj->place.rx = rx;
    obj->place.ry = ry;
    obj->place.rz = rz;
}

int qsort_FaceCompare(const void * f1, const void * f2)
{
    return ((((struct K3DTFace*)f2)->z)-(((struct K3DTFace*)f1)->z));
}

int byteSort(char * array, char * dest, int size)
{
    static int lookup[256];
    static int lookup2[256];
    int i;

    for (i = 255;i >= 0;i--)
        lookup[i] = 0;
    for (i = size;i >= 0;i--)
        lookup[array[i]]++;
    lookup2[0] = lookup[0];
    for (i = 1;i < 255;i++)
        lookup2[i] = lookup[i]+lookup2[i-1];
    for (i = size;i >= 0;i--)
        dest[lookup2[array[i]]] = array[i];
}

void K3DRotateObject(struct K3DTObject *obj)
{
    MATRIX matrix;
    int i;

    get_transformation_matrix(&matrix, obj->scale, obj->place.rx, obj->place.ry, obj->place.rz, obj->place.x, obj->place.y, obj->place.z);
    //  get_translation_matrix(&matrix, obj->place.x, obj->place.y, obj->place.z);

    for (i = 0;i < obj->vertexes;i++)
    {
        apply_matrix(&matrix, obj->vertex[i].x, obj->vertex[i].y, obj->vertex[i].z, &obj->rotated[i].x, &obj->rotated[i].y, &obj->rotated[i].z);
    }
    for (i = 0;i < obj->faces;i++)
    {
        faceDepth[i] = fixtoi(obj->rotated[obj->face[i].v1].z+obj->rotated[obj->face[i].v2].z+obj->rotated[obj->face[i].v3].z)/4;
        obj->face[i].z = obj->rotated[obj->face[i].v1].z+obj->rotated[obj->face[i].v2].z+obj->rotated[obj->face[i].v3].z;
        obj->face[i].color = 255-(faceDepth[i]/2);
        if ((obj->rotated[obj->face[i].v1].z < obj->clipping) ||
                (obj->rotated[obj->face[i].v2].z < obj->clipping) ||
                (obj->rotated[obj->face[i].v3].z < obj->clipping))
            obj->face[i].visible = 0;
        else
            obj->face[i].visible = 1;

    }
    if (obj->rendering.colorModel == Z)
    {
        for (i = 0;i < obj->faces;i++)
        {
            obj->face[i].color = 255-(obj->face[i].z>>16)/8;
            obj->projected[obj->face[i].v1].color =
                obj->projected[obj->face[i].v2].color =
                    obj->projected[obj->face[i].v3].color = obj->face[i].color;
        }
    }

    if (obj->rendering.sorting == QSORT)
    {
        qsort(obj->face, obj->faces, sizeof(K3DTFace), qsort_FaceCompare);
    }
    if (obj->rendering.sorting == BYTESORT)
    {
        byteSort(faceDepth, faceOrder, obj->faces);
    }
}

void K3DProjectObject(struct K3DTObject *obj)
{
    int i;

    for (i = 0;i < obj->vertexes;i++)
    {
        persp_project(obj->rotated[i].x, obj->rotated[i].y, obj->rotated[i].z, &obj->projected[i].x, &obj->projected[i].y);
        //    obj->projected[i].z = obj->rotated[i].z;
    }
}

void K3DDrawObject(BITMAP * buffer, struct K3DTObject *obj)
{
    int i,c;

    if (obj->rendering.filling == PLOT)
    {
        for (i = 0;i < obj->vertexes;i++)                                        /*(obj->projected[i].y>>15)*/
        {
            c = 120+(obj->projected[i].y>>16);
            putpixel(buffer, (obj->projected[i].x>>16), (obj->projected[i].y>>16), c);//(obj->projected[i].y>>16));
            putpixel(buffer, (obj->projected[i].x>>16)+1, (obj->projected[i].y>>16), c);//(obj->projected[i].y>>16));
            putpixel(buffer, (obj->projected[i].x>>16), (obj->projected[i].y>>16)+1, c);//(obj->projected[i].y>>16));
            putpixel(buffer, (obj->projected[i].x>>16)+1, (obj->projected[i].y>>16)+1, c);//(obj->projected[i].y>>16));
        }
    }
    else
        switch (obj->rendering.filling)
        {
        case FLAT:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle(buffer,
                             obj->projected[obj->face[i].v1].x>>16,
                             obj->projected[obj->face[i].v1].y>>16,
                             obj->projected[obj->face[i].v2].x>>16,
                             obj->projected[obj->face[i].v2].y>>16,
                             obj->projected[obj->face[i].v3].x>>16,
                             obj->projected[obj->face[i].v3].y>>16,
                             obj->face[i].color);
            }
            break;
        case GOURAUD:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_GCOL,
                               NULL,
                               (V3D*)&obj->projected[obj->face[i].v1],
                               (V3D*)&obj->projected[obj->face[i].v2],
                               (V3D*)&obj->projected[obj->face[i].v3]
                              );
            }
            break;
        case TEXTURE:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_ATEX,
                               obj->face[i].map,
                               (V3D*)&obj->projected[obj->face[i].v1],
                               (V3D*)&obj->projected[obj->face[i].v2],
                               (V3D*)&obj->projected[obj->face[i].v3]
                              );
            }
            break;
        case TEXTURE_CORRECTED:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_PTEX,
                               obj->face[i].map,
                               (V3D*)&obj->projected[obj->face[i].v1],
                               (V3D*)&obj->projected[obj->face[i].v2],
                               (V3D*)&obj->projected[obj->face[i].v3]
                              );
            }
            break;
        }

}
void K3DRenderObject(BITMAP * buffer, struct K3DTObject *obj)
{
    K3DRotateObject(obj);
    K3DProjectObject(obj);
    K3DDrawObject(buffer, obj);
}

void DrawObject(struct K3DTObject *obj);




float SCALE_X, SCALE_Y;

void Load(void)
{
    void draw_bar(int pos)
    {
        //rectfill(screen, 192+128+16*(pos/32), 270, 14+192+128+16*(pos/32), 286, 32);
    }


    RGB color;
    register int i,x,y;

    PALETTE pal;

    BITMAP *titre = data[BMP_winText].dat;
    BITMAP *menu = data[BMP_winMenu].dat;

    memcpy(pal, data[PAL_winMenu].dat, sizeof(pal));

    //set_gfx_mode(mode, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

    SCALE_X = SCREEN_W/320.0;
    SCALE_Y = SCREEN_H/200.0;

    TunnelLookup = malloc(SCREEN_W*SCREEN_H*sizeof(short));
    TunnelLookup2 = malloc(SCREEN_W*SCREEN_H*sizeof(short));
    PlasmaLookup = malloc(SCREEN_W*SCREEN_H*sizeof(short));
    PlasmaLookup2 = malloc(SCREEN_W*SCREEN_H*sizeof(short));
    _image = malloc(SCREEN_W*SCREEN_H*sizeof(short));

    for (x = 0;x < SCREEN_W;x++)
    {

        for (y = 0;y < SCREEN_H;y++)
        {
            fixed n,m;

            if ((y-SCREEN_H/2) != 0)
                TunnelLookup[SCREEN_W*y+x] = (
                                                 fixtoi(fatan(fdiv(itofix(x-SCREEN_W/2), itofix(y-SCREEN_H/2)))))<<9;
            else
                TunnelLookup[SCREEN_W*y+x] = (128)<<8;

            PlasmaLookup[SCREEN_W*y+x] = TunnelLookup[SCREEN_W*y+x];

            //         TunnelLookup[320*y+x] += x;
            n = itofix(x-SCREEN_W/2);
            m = itofix(y-SCREEN_H/2);

            PlasmaLookup2[SCREEN_W*y+x] = TunnelLookup2[SCREEN_W*y+x] = TunnelLookup[SCREEN_W*y+x]/256;

            if (sqrt((int)(x-SCREEN_W/2)*(x-SCREEN_W/2)+(y-SCREEN_H/2)*(y-SCREEN_H/2)) != 0)
            {
                TunnelLookup[SCREEN_W*y+x] += (SCALE_X*4096)/(int)sqrt((x-SCREEN_W/2)*(x-SCREEN_W/2)+(y-SCREEN_H/2)*(y-SCREEN_H/2));

                PlasmaLookup[SCREEN_W*y+x] += (int)sqrt((x-SCREEN_W/2)*(x-SCREEN_W/2)+(y-SCREEN_H/2)*(y-SCREEN_H/2));
                TunnelLookup2[SCREEN_W*y+x]+= 256*TunnelLookup[SCREEN_W*y+x];
                PlasmaLookup2[SCREEN_W*y+x]+= 256*PlasmaLookup[SCREEN_W*y+x];

            }

        }
    }
    {
        PALETTE pal;

        memcpy(pal,data[PAL_turtle].dat,sizeof(PALETTE));
        for (i = 0;i < 128;i++)
        {
            // picture palette
            pal[i+128].r = pal[i].r;
            pal[i+128].g = pal[i].g;
            pal[i+128].b = pal[i].b;

            // water gradient
            pal[i].r = pal[i].g = i/1;
            pal[i].b = 2*i/1;
        }
        tableWater = malloc(sizeof(COLOR_MAP));
        create_trans_table(tableWater, pal, 128, 128, 128, draw_bar);
    }
    {
        /* load texts */
        for (i=0;i<11;i++)
        {
            BITMAP *_text;
            _text   = create_bitmap(320*SCALE_X,27*SCALE_Y);
            stretch_blit(data[BMP_text].dat,_text,0,i*27,320,27,0,0,320*SCALE_X,27*SCALE_Y);
            text[i] = get_rle_sprite(_text);
        }
    }

    tableGlenz = malloc(sizeof(COLOR_MAP));
    for (i = 0xffff;i >= 0;i--)
    {
        int c;
        c = ((i&255)+(i>>9));
        if (c > 255)
            c = 255;
        *((unsigned char*)tableGlenz+i) = c;
    }

}


typedef struct TDeformTableParam
{
    int amplitude;
    int freq;
    int phase;
}TDeformTableParam;

typedef struct TDeformTable
{
    TDeformTableParam param[4];
    int multiplier;
    int table[256];
}TDeformTable;

void DoDeformTable(TDeformTable* deform, int position)
{
    int i,c;

    // Compute deformation lookup
    for (i = 255;i >= 0;i--)
    {
        c = (
                fsin(itofix(((((i+position)*deform->param[0].freq)>>16)+
                             deform->param[0].phase)&255))+
                fsin(itofix(((((i+position)*deform->param[1].freq)>>16)+
                             deform->param[1].phase)&255))+
                fsin(itofix(((((i+position)*deform->param[2].freq)>>16)+
                             deform->param[2].phase)&255))+
                fsin(itofix(((((i+position)*deform->param[3].freq)>>16)+
                             deform->param[3].phase)&255)));

        // clipping
        deform->table[i] = 128+(fixtoi(c*128/6));
    }
}

static void DrawGlenzTunnel(BITMAP* buffer, unsigned short* lookup, BITMAP* map, BITMAP* picture,
                            int x, int y, TDeformTable * deform)
{
    int offset;
    int i;

    unsigned char* pPicture = _image;

    // offset in map
    offset = x*256+y;

    for (i = 0;i < SCREEN_W*SCREEN_H; i++)
    {
        unsigned char pixel;
        unsigned short address;
        unsigned char save;
        unsigned int def;
        address = lookup[i];
        save = address>>8;
        address+=offset;
        def=deform->table[save];
        address+=def;
        pixel = *(_map+(address&0xffff));
        *(char*)(buffer->dat+i) = *pPicture++ + pixel;
    }

}

void PartTunnel(void)
{
    TDeformTable tunnelDeform;

    BITMAP* map,* picture;
    int i;

    map = data[BMP_map1].dat;
    picture = data[BMP_glenzK].dat;
    set_palette(data[PAL_glenz1].dat);

    // Init global buffers
    for (i = 0;i < SCREEN_W*SCREEN_H;i++)
        *(unsigned char*)(_image+i) = rand()&15;

    for (i = 0;i < 256*256;i++)
        *(unsigned char*)(_map+i) = *((unsigned char*)(map->dat+i))/2;

    tunnelDeform.param[0].amplitude =
        tunnelDeform.param[1].amplitude =
            tunnelDeform.param[2].amplitude =
                tunnelDeform.param[3].amplitude = 0;
    tunnelDeform.param[0].freq = 0x0000;
    tunnelDeform.param[1].freq = 0x0000;
    tunnelDeform.param[2].freq = 0x0000;
    tunnelDeform.param[3].freq = 0x0000;

    while (GetPosition() < 0x1000)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, TunnelLookup, map, picture, VBLframe, VBLframe*2, &tunnelDeform);
        draw_trans_rle_sprite(buffer,text[7],0,SCREEN_H-28*SCALE_Y);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = 192+VBLframe;
        //    tunnelDeform.param[1].phase=tunnelDeform.table[(2*VBLframe)&255]+VBLframe;
        tunnelDeform.param[2].phase = tunnelDeform.table[VBLframe&255]+VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;

        Update();
    }

    tunnelDeform.param[0].amplitude =
        tunnelDeform.param[1].amplitude =
            tunnelDeform.param[2].amplitude =
                tunnelDeform.param[3].amplitude = 1;
    tunnelDeform.param[0].freq = 0x20000;
    tunnelDeform.param[1].freq = 0x30000;
    tunnelDeform.param[2].freq = 0x40000;
    tunnelDeform.param[3].freq = 0x10000;

    while (GetPosition() < 0x1100)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, TunnelLookup, map, picture, VBLframe*0, VBLframe*0, &tunnelDeform);
        draw_trans_rle_sprite(buffer,text[8],0,SCREEN_H-28*SCALE_Y);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = 192+VBLframe;
        //    tunnelDeform.param[1].phase=tunnelDeform.table[(2*VBLframe)&255]+VBLframe;
        tunnelDeform.param[2].phase = tunnelDeform.table[VBLframe&255]+VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;

        Update();
    }
    tunnelDeform.param[0].freq = 0x20000;
    tunnelDeform.param[1].freq = 0x70000;
    tunnelDeform.param[2].freq = 0x40000;
    tunnelDeform.param[3].freq = 0x10000;
    while (GetPosition() < 0x1200)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, TunnelLookup, map, picture, VBLframe, VBLframe*1, &tunnelDeform);
        draw_trans_rle_sprite(buffer,text[9],0,SCREEN_H-28*SCALE_Y);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = 192+VBLframe;
        //    tunnelDeform.param[1].phase=tunnelDeform.table[(2*VBLframe)&255]+VBLframe;
        tunnelDeform.param[2].phase = tunnelDeform.table[VBLframe&255]+VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;

        Update();
    }
    tunnelDeform.param[0].freq = 0x20000;
    tunnelDeform.param[1].freq = 0x10000;
    tunnelDeform.param[2].freq = 0x20000;
    tunnelDeform.param[3].freq = 0x10000;
    while (GetPosition() < 0x1300)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, TunnelLookup, map, picture, VBLframe, VBLframe*2, &tunnelDeform);
        draw_trans_rle_sprite(buffer,text[10],0,SCREEN_H-28*SCALE_Y);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = 192+VBLframe;
        //    tunnelDeform.param[1].phase=tunnelDeform.table[(2*VBLframe)&255]+VBLframe;
        tunnelDeform.param[2].phase = tunnelDeform.table[VBLframe&255]+VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;

        Update();
    }

    BITMAP *tmp = create_bitmap(SCREEN_W, SCREEN_H);
    stretch_blit(data[BMP_glenzK].dat, tmp, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);


    for (i = 0;i < SCREEN_W*SCREEN_H;i++)
        *(unsigned char*)(_image+i) = *((unsigned char*)(tmp->dat+i))/2 + (rand()&15);


    tunnelDeform.param[0].freq = 0x20000;
    tunnelDeform.param[1].freq = 0x10000;
    tunnelDeform.param[2].freq = 0x10000;
    tunnelDeform.param[3].freq = 0x20000;
    while (GetPosition() < 0x1400)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, TunnelLookup2, map, picture, VBLframe*2, VBLframe*0, &tunnelDeform);
        tunnelDeform.param[0].phase = -VBLframe;
        tunnelDeform.param[1].phase = 192+VBLframe;
        tunnelDeform.param[2].phase = 48+VBLframe*2;
        tunnelDeform.param[3].phase = 64-VBLframe;

        Update();
    }
}

void PartInternalBlob(void)
{
    K3DTObject obj;
    BITMAP *map,* texte;
    PALETTE pal;
    int i;

    K3DInit();
    K3DInitObject(&obj);
    K3DLoadObjectASC("blob.asc", &obj);
    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    map = data[BMP_map1].dat;
    memcpy(pal, data[PAL_glenz1].dat, sizeof(PALETTE));
    texte = data[BMP_after].dat;

    K3DSetObjectRendering(&obj, TEXTURE, NONE, Z);
    K3DPlaceObject(&obj, 0, 0, itofix(100), 0, 0, 0);
    K3DSetObjectMap(&obj, map);

    K3DRotateObject(&obj);
    K3DProjectObject(&obj);
    K3DEnvMapObject(&obj);

    color_map = tableGlenz;
    set_palette(pal);
    while (GetPosition() < 0x0300)
    {
        K3DPlaceObject(&obj, 0, itofix(-50),
                       itofix(50)+70*fsin(itofix((1*VBLframe)&255)),
                       (VBLframe<<14)&0xffffff,
                       (VBLframe<<14)&0xffffff,
                       (VBLframe<<14)&0xffffff);

        K3DRotateObject(&obj);
        for (i = 0;i < obj.vertexes;i++)
        {
            obj.rotated[i].x += 20*fsin(itofix((2*i+3*VBLframe)&255));
            obj.rotated[i].y += 20*fsin(itofix((5*i+2*VBLframe)&255));
        }
        K3DProjectObject(&obj);
        K3DDrawObject(buffer, &obj);

        if (GetPosition() >= 0x0100)
            texte = data[BMP_lasy].dat;
        if (GetPosition() >= 0x0200)
            texte = data[BMP_back].dat;
        draw_sprite(buffer, texte, 10, 10);
        
        Update();
    }
    K3DDeleteObject(&obj);
    

    fade_out(12);
    memcpy(pal,data[PAL_kIntro].dat,sizeof(PALETTE));
    stretch_blit(data[BMP_kIntro].dat, screen, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);

    fade_in(pal,2);
    while (GetPosition()<0x0400)
    {
        WaitVBL();
    }
    fade_out(12);
    memcpy(pal,data[PAL_mindIntro].dat,sizeof(PALETTE));
    stretch_blit(data[BMP_mindIntro].dat, screen, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
    //    set_palette(pal);
    fade_in(pal,2);
    while (GetPosition()<0x043e)
    {
        WaitVBL();
    }
    fade_out(12);
  
}

void Part2Blob(void)
{
    K3DTObject obj;
    BITMAP *map;
    PALETTE pal;
    int i;

    K3DInit();
    K3DInitObject(&obj);
    K3DLoadObjectASC("blob.asc", &obj);
    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    map = data[BMP_map1].dat;
    memcpy(pal, data[PAL_glenz1].dat, sizeof(PALETTE));

    K3DSetObjectRendering(&obj, TEXTURE, NONE, Z);
    K3DPlaceObject(&obj, 0, 0, itofix(100), 0, 0, 0);
    K3DSetObjectMap(&obj, map);

    K3DRotateObject(&obj);
    K3DProjectObject(&obj);
    K3DEnvMapObject(&obj);


    //  obj.scale=ftofix(0.3);
    set_palette(pal);
    while (GetPosition() < 0x0a00)
    {
        /* internal blob */
        K3DPlaceObject(&obj, 0, 0,
                       itofix(40)+50*fsin(itofix((4*VBLframe)&255)),
                       (VBLframe<<14)&0xffffff,
                       (VBLframe<<14)&0xffffff,
                       (VBLframe<<14)&0xffffff);

        K3DRotateObject(&obj);
        for (i = 0;i < obj.vertexes;i++)
        {
            obj.rotated[i].x += 24*fsin(itofix((7*i+8*VBLframe)&255))+
                                14*fsin(itofix((6*i+2*VBLframe)&255));
            obj.rotated[i].y += 10*fsin(itofix((1*i+2*VBLframe)&255))+
                                44*fsin(itofix((2*i+5*VBLframe)&255));
        }
        K3DProjectObject(&obj);
        //    K3DEnvMapObject(&obj);
        K3DSetObjectRendering(&obj, TEXTURE, NONE, Z);
        K3DDrawObject(buffer, &obj);

        /* little blob */
        K3DPlaceObject(&obj, 0, 0,
                       itofix(500)+50*fcos(itofix((4*VBLframe)&255)),
                       (VBLframe<<14)&0xffffff,
                       (VBLframe<<14)&0xffffff,
                       (VBLframe<<14)&0xffffff);

        K3DRotateObject(&obj);
        for (i = 0;i < obj.vertexes;i++)
        {
            //      obj.rotated[i].x += 14*fsin(itofix((1*i+1*VBLframe)&255))+
            //                          24*fsin(itofix((2*i+1*VBLframe)&255));
            //      obj.rotated[i].y += 20*fsin(itofix((3*i+3*VBLframe)&255))+
            //                          14*fsin(itofix((7*i+2*VBLframe)&255));
            obj.rotated[i].z += 50*fsin(itofix((3*i+5*VBLframe)&255))+
                                34*fsin(itofix((7*i+8*VBLframe)&255));
        }

        K3DProjectObject(&obj);
        //    K3DEnvMapObject(&obj);

        if (GetPosition() < 0xa00)
            K3DSetObjectRendering(&obj, TEXTURE, QSORT, Z);
        if (GetPosition() < 0x800)
            K3DSetObjectRendering(&obj, GOURAUD, QSORT, Z);
        if (GetPosition() < 0x700)
            K3DSetObjectRendering(&obj, FLAT, QSORT, Z);
        K3DDrawObject(buffer, &obj);

        i = GetPosition();

        if ((i<0x700) && (i>0x600))
            draw_trans_rle_sprite(buffer,text[0],0,SCREEN_H-28*SCALE_Y);
        if ((i<0x800) && (i>0x700))
            draw_trans_rle_sprite(buffer,text[1],0,SCREEN_H-28*SCALE_Y);
        if ((i<0x900) && (i>0x800))
            draw_trans_rle_sprite(buffer,text[2],0,SCREEN_H-28*SCALE_Y);
        if ((i<0xa00) && (i>0x900))
            draw_trans_rle_sprite(buffer,text[3],0,SCREEN_H-28*SCALE_Y);

        if (key[KEY_ESC])
            break;
        Update();
    }
    K3DDeleteObject(&obj);
}


static void DrawWater(BITMAP * buffer, BITMAP * old)
{
    // compute water data

    int i;
    unsigned int b;
    char * table = _map;
    for (i=SCREEN_W*2;i<SCREEN_W*(SCREEN_H-2);i++)
    {
        b = *((unsigned char*)old->dat+i-1)+
            *((unsigned char*)old->dat+i+1)+
            *((unsigned char*)old->dat+i-SCREEN_W)+
            *((unsigned char*)old->dat+i+SCREEN_W);
        b/=2;
        b&=255;
        b=_map[256*b+(*((unsigned char*)buffer->dat+i))*1];
        *((unsigned char*)buffer->dat+i) = b;
    }
    rest(20);
}

void PartWater(void)
{
    BITMAP *old,*_buffer;
    BITMAP *tortue, *tortue2;
    PALETTE pal;
    BITMAP *temp;


    int x,y;
    int i,j;

    // create and clear 2 buffers
    _buffer = create_bitmap(SCREEN_W, SCREEN_H+2);
    clear(_buffer);
    old = create_bitmap(SCREEN_W, SCREEN_H+2);
    clear(old);

    srand(154);

    // load and change a picture
    tortue = data[BMP_turtle].dat;
    memcpy(pal,data[PAL_turtle].dat,sizeof(PALETTE));
    for (x = 0;x < 172;x++)
        for (y = 0;y < 256;y++)
            _putpixel(tortue, x, y, _getpixel(tortue, x, y)+128);

    for (i = 0;i < 128;i++)
    {
        // picture palette
        pal[i+128].r = pal[i].r;
        pal[i+128].g = pal[i].g;
        pal[i+128].b = pal[i].b;

        // water gradient
        pal[i].r = pal[i].g = i/1;
        pal[i].b = 2*i/1;
    }
    set_palette(pal);

    color_map = tableWater;

    tortue2 = create_bitmap(172*SCALE_X, 256*SCALE_Y);

    stretch_blit(tortue, tortue2, 0, 0, 172, 256, 0, 0,172*SCALE_X, 256*SCALE_Y);

    // calculate the lookup
    for (x = 0;x < 256;x++)
        for (y = 0;y < 256;y++)
        {
            j = y-x;
            j -= j/16;
            if (j < 0)
                j = 0;
            _map[x+256*y] = j;
        }

    VBLframe = retrace_count = 200;
    x = 0;
    x =-300*SCALE_X;
    while (GetPosition() < 0x1700)
    {
        // swap new and old buffer
        temp = _buffer;
        _buffer = old;
        old = temp;

        if (GetPosition() == 0x1500)
        {
            x = -175*SCALE_X;
        }
        if ((x>-176*SCALE_X) && (x<=0))
            x+=SCALE_X;

        // put the picture
        //      blit(tortue,buffer,x,(fsin((itofix((4*VBLframe)&255)))/2340)+27,0,0,172,256);
        blit(_buffer, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        draw_trans_sprite(buffer, tortue2, x, (fsin((itofix((10*VBLframe)&255)))/2340)-27);

        Update();

        // adjust contours
        /*line(old, 0, 0, 319, 0, 0);
        line(old, 0, 0, 0, 199, 0);
        line(old, 319, 0, 319, 199, 0);
        line(old, 0, 199, 319, 199, 0);
        */
        // draw water plot motion
        _putpixel(old, SCREEN_W/2+SCREEN_W/3*cos(2*3.14159*VBLframe/256), SCREEN_H/2+SCREEN_H/3*sin(4*3.14159*VBLframe/256), 127);
        putpixel(old, rand()%(SCREEN_W-1), rand()%(SCREEN_H-1), 127);

        // and calculate it
        DrawWater(_buffer, old);

    }
}


void PartMetaballs(void)
{
    K3DTObject obj;
    BITMAP *map,*blob,*back;
    PALETTE pal;
    int i,c,x,y;

    K3DInit();
    K3DInitObject(&obj);
    K3DLoadObjectASC("k.asc", &obj);
    blob = create_bitmap(SCALE_X*64, SCALE_Y*64);
    clear(blob);

    for (y = 0;y < SCALE_Y*64;y++)
    {
        for (x = 0;x < SCALE_X*64;x++)
        {
            int cx,cy;
            cx = x - SCALE_X*64/2;
            cy = y - SCALE_Y*64/2;
            float dist = 256 - 8/SCALE_Y * sqrt(cx*cx+cy*cy);
            if (dist > 255) dist = 255;
            if (dist < 0) dist = 0;
            putpixel(blob, x,y, dist);
        }
    }

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    map = data[BMP_map1].dat;
    back = create_bitmap(SCREEN_W, SCREEN_H);
    data[BMP_glenzMind].dat;
    stretch_blit(data[BMP_glenzMind].dat, back, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);


    memcpy(pal, data[PAL_glenz3].dat, sizeof(PALETTE));

    K3DSetObjectRendering(&obj, TEXTURE, QSORT, NONE);
    K3DPlaceObject(&obj, 0, 0, itofix(100), 0, 0, 0);
    K3DSetObjectMap(&obj, map);

    K3DRotateObject(&obj);
    K3DProjectObject(&obj);
    K3DEnvMapObject(&obj);

    color_map = tableGlenz;

    //  obj.scale=ftofix(0.3);
    set_palette(pal);
    while (GetPosition() < 0x0c00)
    {
        K3DPlaceObject(&obj, 0, 0,
                       itofix(120)+30*fsin(itofix((1*VBLframe)&255)),
                       itofix(64), 0, ((256-VBLframe)<<17)&0xffffff);
        //                   (VBLframe<<16)&0xffffff,
        //                   (VBLframe<<16)&0xffffff);


        stretch_blit(buffer, buffer, 0, 0, SCREEN_W, SCREEN_H, 0, 0, (SCREEN_W/4), (SCREEN_H/4));
        rect(buffer, 0, 0, SCREEN_W/4, SCREEN_H/4, 128);
        Update();
        blit(back, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        //    clear(buffer);
        K3DRotateObject(&obj);
        K3DProjectObject(&obj);
        K3DEnvMapObject(&obj);

        c = 8;
        for (i = 0;i < c;i++)
            draw_trans_sprite(buffer, blob,
                              SCALE_X*160-SCALE_X*32+fixtoi(SCALE_X*60*fsin(itofix((VBLframe+i*256/c)&255)))+fixtoi(SCALE_X*80*fcos(itofix((VBLframe*2+i*256/c)&255)))
                              , SCALE_Y*100-SCALE_Y*32+fixtoi(SCALE_Y*70*fcos(itofix((VBLframe*3+i*256/c)&255)))+fixtoi(SCALE_Y*40*fsin(itofix((VBLframe+i*256*c)&255))));

        K3DDrawObject(buffer, &obj);

        c = 6;
        for (i = 0;i < c;i++)
            draw_trans_sprite(buffer, blob,
                              SCALE_X*160-SCALE_X*32+fixtoi(SCALE_X*60*fsin(itofix((VBLframe+i*256/c)&255)))+fixtoi(SCALE_X*80*fcos(itofix((VBLframe+i*256/c)&255)))
                              , SCALE_Y*100-SCALE_Y*32+fixtoi(SCALE_Y*80*fcos(itofix((VBLframe+i*256/c)&255)))+fixtoi(SCALE_Y*30*fsin(itofix((VBLframe*2+i*256*c)&255))));


        draw_trans_rle_sprite(buffer,text[4],0,SCREEN_H-28*SCALE_Y);

        if (key[KEY_ESC])
            break;
    }
    K3DDeleteObject(&obj);
    destroy_bitmap(blob);
}

void PartTunnel3d(void)
{
    K3DTObject obj;
    BITMAP *map,*tv;
    PALETTE pal;

    K3DInit();
    K3DInitObject(&obj);
    K3DLoadObjectASC("tunnel.asc", &obj);
    obj.clipping = itofix(2);

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    map = data[BMP_map1].dat;
    memcpy(pal, data[PAL_glenz2].dat, sizeof(PALETTE));

    tv = create_bitmap(80*SCALE_X, 50*SCALE_Y);

    K3DSetObjectRendering(&obj, TEXTURE, NONE, Z);
    K3DPlaceObject(&obj, 0, 0, itofix(100*SCALE_X), 0, 0, 0);
    K3DSetObjectMap(&obj, map);

    K3DRotateObject(&obj);
    K3DProjectObject(&obj);
    K3DEnvMapObject(&obj);

    //  obj.scale=ftofix(0.3);
    set_palette(pal);
    while (GetPosition() < 0x0d00)
    {
        clear(buffer);
        K3DPlaceObject(&obj,
                       0,
                       0,
                       0,
                       (VBLframe<<16)&0xffffff,
                       (VBLframe<<16)&0xffffff,
                       (VBLframe<<16)&0xffffff);


        blit(tv, buffer, 0, 0, 0, 0, 80*SCALE_X, 50*SCALE_Y);
        rotate_sprite(buffer, tv, SCREEN_W-80*SCALE_X, SCREEN_H-50*SCALE_Y, VBLframe<<16);
        //    rect(buffer,0,0,SCREEN_W/4,SCREEN_H/4,128);
        line(buffer, 0, 50*SCALE_Y, SCREEN_W-1, 50*SCALE_Y, 128);
        line(buffer, 80*SCALE_X, 0, 80*SCALE_X, SCREEN_H-1, 128);

        stretch_blit(buffer, tv, 0, 0, SCREEN_W, SCREEN_H, 0, 0, (SCREEN_W/4), (SCREEN_H/4));
        K3DRotateObject(&obj);
        K3DProjectObject(&obj);
        //    K3DEnvMapObject(&obj);

        K3DDrawObject(buffer, &obj);

        while (key[KEY_F1])
        {
        }

        draw_trans_rle_sprite(buffer,text[5],0,SCREEN_H-28*SCALE_Y);

        Update();
    }
    K3DDeleteObject(&obj);
}

void PartTunnel3d2(void)
{
    K3DTObject obj;
    BITMAP *map;
    PALETTE pal;
    int i;

    K3DInit();
    K3DInitObject(&obj);
    K3DLoadObjectASC("tube.asc", &obj);

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);
    map = data[BMP_map1].dat;
    memcpy(pal, data[PAL_glenz3].dat, sizeof(PALETTE));

    K3DSetObjectRendering(&obj, TEXTURE, NONE, NONE);
    K3DPlaceObject(&obj, 0, 0, itofix(100*SCALE_X), 0, 0, 0);
    K3DSetObjectMap(&obj, map);

    K3DRotateObject(&obj);
    K3DProjectObject(&obj);
    K3DEnvMapObject(&obj);

    VBLframe = retrace_count =-200;
    obj.clipping = itofix(40);
    //  obj.scale=ftofix(0.3);
    set_palette(pal);
    while (GetPosition() < 0x0f00)
    {
        clear(buffer);

        for (i = 4;i >= 0;i--)
        {
            K3DPlaceObject(&obj,
                           400*65536*sin(2*3.1415927*VBLframe/256),
                           0,
                           itofix((512*i-(VBLframe*14))&2047),
                           itofix(64),
                           //                     ((i*32-VBLframe)<<16)&0xffffff,
                           128*65536*sin(2*3.1415927*(VBLframe+i*9)/256),
                           itofix(0));
            K3DRotateObject(&obj);
            K3DProjectObject(&obj);
            //      K3DEnvMapObject(&obj);

            K3DDrawObject(buffer, &obj);
        }

        if (GetPosition() < 0x0d00)
            draw_trans_rle_sprite(buffer,text[5],0,SCREEN_H-28*SCALE_Y);
        else
            draw_trans_rle_sprite(buffer,text[6],0,SCREEN_H-28*SCALE_Y);

        Update();
    }
    K3DDeleteObject(&obj);
}

void PartPlasma(void)
{
    TDeformTable tunnelDeform;

    BITMAP* map,* picture;
    PALETTE pal,black,begin;
    int i,fade;
    BITMAP* overlay;

    map = data[BMP_map1].dat;
    picture = data[BMP_glenzK].dat;
    set_palette(data[PAL_glenz1].dat);

    memcpy(begin,data[PAL_glenz1].dat,sizeof(PALETTE));
    for (i = 0;i < 256;i++)
        black[i].r=black[i].g=black[i].b=0;

    overlay = create_bitmap(SCREEN_W, SCREEN_H);
    stretch_blit(data[BMP_think].dat, overlay, 0,0,320,200,0,0,SCREEN_W, SCREEN_H);

    // Init global buffers
    for (i = 0;i < 256*256;i++)
        *(unsigned char*)(_map+i) = *((unsigned char*)(map->dat+i))/2;
    for (i = 0;i < SCREEN_W*SCREEN_H;i++)
        *(unsigned char*)(_image+i) = rand()&15;

    tunnelDeform.param[0].freq = 0x10000;
    tunnelDeform.param[1].freq = 0x10000;
    tunnelDeform.param[2].freq = 0x10000;
    tunnelDeform.param[3].freq = 0x10000;

    while (GetPosition() < 0x1800)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, PlasmaLookup, map, picture,
                        tunnelDeform.table[(VBLframe+194)&255],
                        tunnelDeform.table[(2*VBLframe)&255], &tunnelDeform);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = tunnelDeform.table[(5*VBLframe)&255];
        tunnelDeform.param[2].phase = tunnelDeform.table[VBLframe&255]+VBLframe;
        tunnelDeform.param[3].phase = 192+VBLframe;
        Update();
    }
    tunnelDeform.param[0].freq = 0x10000;
    tunnelDeform.param[1].freq = 0x20000;
    tunnelDeform.param[2].freq = 0x20000;
    tunnelDeform.param[3].freq = 0x10000;
    while (GetPosition() < 0x1900)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, PlasmaLookup, map, picture,
                        tunnelDeform.table[(VBLframe+194)&255],
                        tunnelDeform.table[(2*VBLframe)&255], &tunnelDeform);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = tunnelDeform.table[(2*VBLframe)&255];
        tunnelDeform.param[2].phase = tunnelDeform.table[VBLframe&255]+VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;

        Update();
    }
    while (GetPosition() < 0x1a00)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, PlasmaLookup2, map, picture,
                        tunnelDeform.table[(VBLframe+194)&255],
                        tunnelDeform.table[(2*VBLframe)&255], &tunnelDeform);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = tunnelDeform.table[(2*VBLframe)&255];
        tunnelDeform.param[2].phase = VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;
        Update();
    }
    tunnelDeform.param[3].freq = 0x30000;
    while (GetPosition() < 0x1c00)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, PlasmaLookup, map, picture,
                        tunnelDeform.table[(VBLframe+194)&255],
                        tunnelDeform.table[(2*VBLframe)&255], &tunnelDeform);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = tunnelDeform.table[(2*VBLframe)&255];
        tunnelDeform.param[2].phase = VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;
        Update();
    }
    for (i = 0;i < SCREEN_W*SCREEN_H;i++)
        *(unsigned char*)(_image+i) = 70+*((unsigned char*)(overlay->dat+i))/2;
    while (GetPosition() < 0x1d00)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, PlasmaLookup2, map, picture,
                        tunnelDeform.table[(VBLframe+194)&255],
                        tunnelDeform.table[(2*VBLframe)&255], &tunnelDeform);
        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = tunnelDeform.table[(2*VBLframe)&255];
        tunnelDeform.param[2].phase = VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;
        Update();
    }
    fade=0;
    while (GetPosition() < 0x1e00)
    {
        DoDeformTable(&tunnelDeform, VBLframe);
        DrawGlenzTunnel(buffer, PlasmaLookup, map, picture,
                        tunnelDeform.table[(VBLframe+194)&255],
                        tunnelDeform.table[(2*VBLframe)&255], &tunnelDeform);
        if (fade<64*3)
            fade_interpolate(begin, black, pal, fade/3, 0, 255);
        //vsync();
        fade++;
        set_palette(pal);

        tunnelDeform.param[0].phase = 64+VBLframe;
        tunnelDeform.param[1].phase = tunnelDeform.table[(2*VBLframe)&255];
        tunnelDeform.param[2].phase = VBLframe;
        tunnelDeform.param[3].phase = 274+VBLframe;
        Update();
    }

}

void PartEnd(void)
{
    K3DTObject obj;
    int i;
    float pos;
    PALETTE black,pal;
    BITMAP *back;
    BITMAP *end;

    K3DInit();
    K3DInitObject(&obj);
    K3DLoadObjectASC("blob.asc", &obj);
    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);

    K3DSetObjectRendering(&obj, FLAT, QSORT, Z);
    K3DPlaceObject(&obj, 0, 0, itofix(100), 0, 0, 0);
    K3DSetObjectMap(&obj, data[BMP_map1].dat);

    K3DRotateObject(&obj);
    K3DProjectObject(&obj);
    K3DEnvMapObject(&obj);

    for (i=0;i<255;i++)
        black[i].r=black[i].g=black[i].b=0;

    back = create_bitmap(SCREEN_W, SCREEN_H);
    stretch_blit(data[BMP_bumpBack].dat, back, 0,0,320,200,0,0,SCREEN_W, SCREEN_H);
    end = create_bitmap(SCREEN_W, SCREEN_H);
    stretch_blit(data[BMP_kEnd].dat, end, 0,0,320,200,0,0,SCREEN_W, SCREEN_H);

    obj.scale=ftofix(0.5);

    MusicSetPosition(0x1f);

    color_map = tableGlenz;
    set_palette(data[PAL_glenz2].dat);
    pos = SCREEN_H;

    while (pos > -1000)   // for(i=VBLframe; i<800*2; i=VBLframe)
    {
        pos -= delta;
        Update();
        blit(back,buffer,0,0,0,0,SCREEN_W,SCREEN_H);

        K3DPlaceObject(&obj,itofix(250)+100*fsin(itofix(VBLframe&255)),itofix((VBLframe&1023)-512),itofix(300),itofix(VBLframe),itofix(0),itofix(VBLframe));
        K3DRenderObject(buffer, &obj);

        K3DPlaceObject(&obj,itofix(250)+100*fcos(itofix(VBLframe&255)),itofix(((VBLframe+512)&1023)-512),itofix(300),itofix(VBLframe),itofix(0),itofix(VBLframe));
        K3DRenderObject(buffer, &obj);

        draw_trans_rle_sprite(buffer,data[BMP_scroll].dat,SCREEN_W/2,pos);

        if (key[KEY_ESC])
            return;
    }
    for (i=0;i<64*2;i++)
    {
        fade_interpolate(data[PAL_glenz1].dat, black, pal, i/2, 0, 255);
        Update();
        vsync();
        set_palette(pal);
        blit(end,buffer,0,0,0,0,SCREEN_W,SCREEN_H);

        K3DPlaceObject(&obj,itofix(250)+100*fsin(itofix(VBLframe&255)),itofix(((VBLframe)&1023)-512),itofix(300),itofix(VBLframe),itofix(0),itofix(VBLframe));
        K3DRenderObject(buffer, &obj);

        K3DPlaceObject(&obj,itofix(250)+100*fcos(itofix(VBLframe&255)),itofix(((VBLframe+512)&1023)-512),itofix(300),itofix(VBLframe),itofix(0),itofix(VBLframe));
        K3DRenderObject(buffer, &obj);

        if (key[KEY_ESC])
            return;
    }

    K3DDeleteObject(&obj);
}

void Demo(void)
{
    //set_gfx_mode(mode, 320, 200, 0, 0);
    buffer = create_bitmap(SCREEN_W, SCREEN_H);

    PartInternalBlob();
    Part2Blob();
    PartMetaballs();
    PartTunnel3d();
    PartTunnel3d2();
    PartTunnel();
    PartWater();
    PartPlasma();
    PartEnd();
}



int main(int argc, char ** argv)
{
    char buf[80];
    int refreshRate;
    int card, w, h;
    int mode=GFX_AUTODETECT_WINDOWED;

    printf("MindLink (c) KNIGHTS 1997-2009\n");
    printf("  mindlink WIDTHxHEIGHT\n");
    printf("  Initializing.\n");

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    allegro_init();
    install_keyboard();
    install_mouse();

    // mode = GFX_AUTODETECT_FULLSCREEN;

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
        set_gfx_mode(mode, 320, 240, 0, 0);
        gfx_mode_select(&card, &w, &h);
        set_gfx_mode(card, w,h, 0,0);
    }

    /* load the datafile into memory */
    printf("  UnPacking.");
    fflush(stdout);
    data = load_datafile("mindlink.dat");
    if (!data)
    {
        allegro_exit();
        printf("  Error loading example.dat!\n\n");
        exit(1);
    }

    printf(".");
    fflush(stdout);
    //install_timer();

    printf(".");
    fflush(stdout);

    Load();

    printf(".");
    fflush(stdout);

    MusicStart("mindlink.xm");

    SDL_AddTimer(100, MusicUpdate, 0);
    SDL_AddTimer(1000,TimerHandler, 0);


    DumpStart();
    Demo();
    DumpEnd();

    MusicStop();

    //MusicDump("mindlink.xm");

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

    printf("\nMindLink 1.1 (GPL) [%s %s]\n\n", __DATE__, __TIME__);
    printf("                     \n");
    printf("\n");


    //system("mencoder "mf://*.jpg" -mf fps=10 -o test.avi -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=800");

    return 0;
}
END_OF_MAIN()
