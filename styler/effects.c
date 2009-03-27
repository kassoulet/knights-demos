/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  02/98

  (c) KNIGHTS 1998

  Effects

*/


#include "effects.h"

extern float SCALE_X;
extern float SCALE_Y;
char _map[256*256];
char *_image;

void SetTunnelMap(BITMAP * bmp, BITMAP * img)
{
    int x,y;
    BITMAP *tmp;

    for (x = 0;x < 256;x++)
        for (y = 0;y < 256;y++)
            _map[y*256+x] = _getpixel(bmp, x, y);
            
    _image = malloc(SCREEN_W*SCREEN_H);
    tmp = create_bitmap(SCREEN_W, SCREEN_H);
    stretch_blit(img, tmp, 0, 0, 320, 200, 0, 0, SCREEN_W, SCREEN_H);
    for (x = 0;x < SCREEN_W;x++)
        for (y = 0;y < SCREEN_H;y++)
            _image[y*SCREEN_W+x] = _getpixel(tmp, x, y);
    destroy_bitmap(tmp);
}


void CalcTunnelLookup(unsigned short* lookup, int orientation, int deep)
{
    int x,y;

    for (x = 0;x < SCREEN_W;x++)
    {
        for (y = 0;y < SCREEN_H;y++)
        {
            fixed n,m;
            if ((y-SCREEN_H/2) != 0)
                lookup[SCREEN_W*y+x] = (fixtoi(fatan(fdiv(itofix(x-SCREEN_W/2), itofix(y-SCREEN_H/2)))))<<9;
            else
                lookup[SCREEN_W*y+x] = (128)<<8;
            n = itofix(x-SCREEN_W/2);
            m = itofix(y-SCREEN_H/2);
            if (sqrt((int)(x-SCREEN_W/2)*(x-SCREEN_W/2)+(y-SCREEN_H/2)*(y-SCREEN_H/2)) != 0)
            {
                if (deep)
                    lookup[SCREEN_W*y+x] += (SCALE_X*4096)/(int)sqrt((x-SCREEN_W/2)*(x-SCREEN_W/2)+(y-SCREEN_H/2)*(y-SCREEN_H/2));
                else
                    lookup[SCREEN_W*y+x] += (int)sqrt((x-SCREEN_W/2)*(x-SCREEN_W/2)+(y-SCREEN_H/2)*(y-SCREEN_H/2));
            }
            if (orientation)
            {
                char temp;
                temp = lookup[SCREEN_W*y+x]/256;
                lookup[SCREEN_W*y+x] = lookup[SCREEN_W*y+x]*256+temp;
            }
        }
    }
}

void DrawTunnel(BITMAP* buffer, unsigned short* lookup, int x, int y, TDeformTable * deform)
{
    int offset;
    int i;

    unsigned char* pPicture = _image;

    // offset in map
    offset = (x&255)*256+(y&255);
    for (i = 0;i < SCREEN_W*SCREEN_H;i++)
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


void DoDeformTable(TDeformTable* deform, int position)
{
    int i,c;

    // Compute deformation lookup
    for (i=0; i<256; i++)
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


void Threshold(BITMAP *dest, BITMAP *img, BITMAP *height, int threshold)
{
    register int i;
    register unsigned char * d,*h,*s;
    unsigned char table[256];

    threshold &= 255;

    for (i = 0;i < threshold;i++)
    {
        table[i] = 255;
    }
    for (;i < 256;i++)
    {
        table[i] = 0;
    }


    h = (unsigned char*)height->dat;
    d = (unsigned char*)dest->dat;
    s = (unsigned char*)img->dat;

    for (i = img->w*img->h-1;i >= 0;i--)
    {
        *d = *s&table[*h];
        d++;
        s++;
        h++;
    }
}

void ThresholdSmooth(BITMAP *dest, BITMAP *img, BITMAP *height, int threshold)
{
    register int i;
    register unsigned char * d,*h,*s;
    unsigned char table[256];
    int n;

    h = (unsigned char*)height->dat;
    d = (unsigned char*)dest->dat;
    s = (unsigned char*)img->dat;

    if (threshold > 245)
    {
        blit(img, dest, 0, 0, 0, 0, img->w, img->h);
        return;
    }

    for (i = 0;i < threshold-10;i++)
    {
        table[i] = 255;
    }
    n = 255;
    for (;i < threshold+10;i++)
    {
        table[i] = n;
        n -= 10;
    }
    for (;i < 256;i++)
    {
        table[i] = 0;
    }

    for (i = img->w*img->h-1;i >= 0;i--)
    {
        *d = color_map->data[table[*h]][*s];
        d++;
        s++;
        h++;
    }
}

void InitAim(K3DTObject *aim)
{
    // 3DS SUXX !!!!!! so I build the object myself !!!

    aim->vertex = (struct K3DTVertex*) malloc(7*sizeof(struct K3DTVertex));
    aim->rotated = (struct K3DTVertex*) malloc(7*sizeof(struct K3DTVertex));
    aim->projected = (struct K3DTVertex*) malloc(7*sizeof(struct K3DTVertex));
    aim->face = (struct K3DTFace*) malloc(3*sizeof(struct K3DTFace));

    aim->vertex[0].x = itofix(0);
    aim->vertex[0].y = itofix(-100);
    aim->vertex[0].z = 0;

    aim->vertex[1].x = itofix(-20);
    aim->vertex[1].y = itofix(-120);
    aim->vertex[1].z = 0;

    aim->vertex[2].x = itofix(20);
    aim->vertex[2].y = itofix(-120);
    aim->vertex[2].z = 0;

    aim->vertex[3].x = itofix(-10);
    aim->vertex[3].y = itofix(-120);
    aim->vertex[3].z = 0;

    aim->vertex[4].x = itofix(10);
    aim->vertex[4].y = itofix(-120);
    aim->vertex[4].z = 0;

    aim->vertex[5].x = itofix(-10);
    aim->vertex[5].y = itofix(-140);
    aim->vertex[5].z = 0;

    aim->vertex[6].x = itofix(10);
    aim->vertex[6].y = itofix(-140);
    aim->vertex[6].z = 0;

    aim->face[0].v1 = 0;
    aim->face[0].v2 = 1;
    aim->face[0].v3 = 2;

    aim->face[1].v1 = 5;
    aim->face[1].v2 = 4;
    aim->face[1].v3 = 3;

    aim->face[2].v1 = 6;
    aim->face[2].v2 = 4;
    aim->face[2].v3 = 5;

    aim->vertexes = 7;
    aim->faces = 3;
}


void drawRotatedBack(BITMAP * buffer, BITMAP * back, fixed angle)
{
    V3D points[4];
    int s,z;
    int size = 255;

    s = (itofix(420*SCALE_X)+(int)(150*SCALE_X*fsin(angle)))>>16;

    points[0].x = itofix(0);
    points[0].y = itofix(0);
    points[0].x = (s*fcos(angle))+itofix(SCREEN_W/2);
    points[0].y = (s*fsin(angle))+itofix(SCREEN_H/2);
    points[0].z = z;
    points[0].u = 0;
    points[0].v = 0;
    points[0].c = (itofix(127)+127*fsin(1*angle))>>16;;;

    points[1].x = itofix(320);
    points[1].y = itofix(0);
    points[1].x = (s*fcos(angle+itofix(64)))+itofix(SCREEN_W/2);
    points[1].y = (s*fsin(angle+itofix(64)))+itofix(SCREEN_H/2);
    points[1].z = z;
    points[1].u = size<<16;
    points[1].v = 0;
    points[1].c = (itofix(192)+63*fsin(2*angle))>>16;

    points[2].x = itofix(320);
    points[2].y = itofix(200);
    points[2].x = (s*fcos(angle+itofix(128)))+itofix(SCREEN_W/2);
    points[2].y = (s*fsin(angle+itofix(128)))+itofix(SCREEN_H/2);
    points[2].z = z;
    points[2].u = size<<16;
    points[2].v = size<<16;
    points[2].c = (itofix(127)+127*fsin(3*angle))>>16;;;

    points[3].x = itofix(0);
    points[3].y = itofix(200);
    points[3].x = (s*fcos(angle+itofix(192)))+itofix(SCREEN_W/2);
    points[3].y = (s*fsin(angle+itofix(192)))+itofix(SCREEN_H/2);
    points[3].z = z;
    points[3].u = 0;
    points[3].v = size<<16;
    points[3].c = (itofix(192)+63*fsin(angle/3))>>16;

    quad3d(buffer, POLYTYPE_ATEX, back, &points[0], &points[1], &points[2], &points[3]);
}

BITMAP * CreateDeformBitmap(BITMAP *img)
{
    BITMAP * bmp;
    int x,y;

    bmp = img;
    //  bmp = create_bitmap(img->w,img->h);
    for (y = 0;y <= (img->h-1);y++)
        for (x = 0;x < img->w;x++)
            _putpixel(bmp, x, y, abs(_getpixel(img, x+1, y)
                                     /*-getpixel(img,x,y)*/));
    return bmp;
}

void DeformBitmap(BITMAP *dest, BITMAP *src, BITMAP *deform)
{
    int x,y;

    for (y = 0;y < dest->h;y++)
        for (x = 0;x < dest->w;x++)
            _putpixel(dest, x, y, _getpixel(src, x+_getpixel(deform, x, y), y));
}


void DeformBitmapScroll(BITMAP *dest,BITMAP *src,BITMAP *deform,int dx,int dy,int ix,int iy)
{
    int x,y;

    for (y=0;y<dest->h;y++)
        for (x=0;x<dest->w;x++)
            _putpixel(dest,x,y,_getpixel(src,
            	(ix+x+_getpixel(deform,(dx+x)&511,(dy+y)&511)) & 511,
            	(y+iy) & 511));
}




void DrawPlasma(BITMAP * buffer, BITMAP * image, int frame, int amplitude, int deform)
{
    // image must be 450x330 !!!

    int i,y;

    unsigned long sinus[320+200];

    if (deform)
    {
        for (i = 0;i < 320;i++)
            sinus[i] = fixtoi(
                           itofix(64)+
                           ((amplitude*fsin(itofix((i+frame/1)&255))
                             +amplitude*fcos(itofix((frame/4)&255))
                             +amplitude*fsin(itofix((frame/2-i)&255))
                             +amplitude*fcos(itofix((frame/4+i/3)&255))
                            )/16))*450;

        for (i = 0;i < 200;i++)
            sinus[i+320] = fixtoi(
                               itofix(64)+
                               ((amplitude*fsin(itofix((i+frame/9)&255))
                                 +amplitude*fcos(itofix((frame/3)&255))
                                 +amplitude*fsin(itofix((frame+i)&255))
                                 +amplitude*fcos(itofix((frame*3+i/7)&255))
                                )/16));
    }
    else
    {
        for (i = 0;i < 320;i++)
            sinus[i] = fixtoi(
                           itofix(64)+
                           ((amplitude*fsin(itofix((i+frame/1)&255))
                             +amplitude*fcos(itofix((frame/4)&255))
                             +amplitude*fsin(itofix((frame/2-i)&255))
                             +amplitude*fcos(itofix((frame/4+i/3)&255))
                            )/16));

        for (i = 0;i < 200;i++)
            sinus[i+320] = fixtoi(
                               itofix(64)+
                               ((amplitude*fsin(itofix((i+frame/9)&255))
                                 +amplitude*fcos(itofix((frame/3)&255))
                                 +amplitude*fsin(itofix((frame+i)&255))
                                 +amplitude*fcos(itofix((frame*3+i/7)&255))
                                )/16))*450;
    }

    {
        unsigned char *dest,*img;
        int x;

        dest = (unsigned char *)buffer->dat;
        img = (unsigned char *)image->dat;

        for (y = 199;y >= 0;y--)
        {
            unsigned char *D,*S;
            D=dest+320*y;
            S=img+450*y;
            for (x=0;x<320;x++)
            {
                *D=*(S+sinus[x]+sinus[320+y]);
                S++;
                D++;
            }
        }
    }
}

/*



WARNING
*/

void DrawWater(BITMAP * buffer, BITMAP * old)
{
    // compute water data

    int i;
    unsigned int b;

    for (i=640;i<320*197;i++)
    {
        b = *((unsigned char*)old->dat+i-1)+
            *((unsigned char*)old->dat+i+1)+
            *((unsigned char*)old->dat+i-320)+
            *((unsigned char*)old->dat+i+320);
        b/=2;
        b=_map[256*b+*((unsigned char*)buffer->dat+i)];
        *((unsigned char*)buffer->dat+i) = b;
    }

}

/* don't really work... */
void CalcWater(void)
{
    int x,y,j;

    // calculate the lookup
    for (x = 0;x < 256;x++)
        for (y = 0;y < 256;y++)
        {
            j = (y-128)-(x-128);
            j -= j/64;
            if (j < 0)
                j = 0;
            _map[y+256*x] = j+128;
        }
}

BITMAP * bumpTable,*bumpDelta;


void CalcBump(BITMAP * map)
{
    int x,y;

    bumpTable = create_bitmap(256, 256);
    bumpDelta = create_bitmap(640, 200);

    for (y = 1;y < 198;y++)
        for (x = 1;x < 318;x++)
        {
            *(bumpDelta->line[y]+2*x) = *(map->line[y]+x+1)-*(map->line[y]+x-1);;
            *(bumpDelta->line[y]+2*x+1) = *(map->line[y+1]+x)-*(map->line[y-1]+x);;
        }

    for (y = 0;y < 256;y++)
        for (x = 0;x < 256;x++)
        {
            int tx,ty,dist;

            tx = x-127;
            ty = y-127;
            dist = sqrt(tx*tx+ty*ty);
            if (dist < 128)
                putpixel(bumpTable, x, y, (char)(127-(dist)));
            else
                putpixel(bumpTable, x, y, 1);
            if (getpixel(bumpTable, x, y) < 1)
                putpixel(bumpTable, x, y, 1);
        }
}

/* bugged! no time to fix :( */
void DrawBump(BITMAP * buffer, int x1, int y1)
{
    int x,y;
    int dx,dy,vx,vy,c,x2,y2,tx,ty;

    x2 = x1+256;
    y2 = y1+256;
    tx = x1;
    ty = y1;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x1 > 319) x1 = 319;
    if (y1 > 199) y1 = 199;
    if (x2 < 0) x2 = 0;
    if (y2 < 0) y2 = 0;
    if (x2 > 319) x2 = 319;
    if (y2 > 199) y2 = 199;

    /*
    rectfill(buffer, 0, 0, x1, y2, 0);
    rectfill(buffer, 0, y2, x2, 199, 0);
    rectfill(buffer, x1, 0, 319, y1, 0);
    rectfill(buffer, x2, y1, 319, 199, 0);
    */
    clear(buffer);

    for (y = y1+1;y < y2-1;y++)
    {
        vx = x-tx;
        vy = (int)y-ty;
        for (x = x1+1;x < x2;x++)
        {
            //          vx=x-tx;
            dx = (char) *(bumpDelta->line[y]+2*x);
            dy = (char) *(bumpDelta->line[y]+2*x+1);
            c = (char) *(bumpTable->line[(dy+vy)&255]+((dx+(vx++))&255));

            *(buffer->line[y]+x) = c;
        }
    }

    rect(buffer, x1, y1, x2, y2, 127);
}



