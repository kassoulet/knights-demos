/*

  Styler 2

  Coded by Gautier "Impulse" PORTET

  01/98  03/98

  (c) KNIGHTS 1998

  Effects

*/


#include "effects.h"


char _map[256*256];
char *_image;

void SetTunnelMap(BITMAP * bmp)
{
    int x,y;

    for (x = 0;x < 256;x++)
        for (y = 0;y < 256;y++)
            _map[y*256+x] = getpixel(bmp, x, y);
}


void CalcTunnelLookup(unsigned short* lookup, int orientation, int deep)
{
    int x,y;

    for (x = 0;x < 640;x++)
    {
        for (y = 0;y < 400;y++)
        {
            fixed n,m;
            if ((y-200) != 0)
                lookup[640*y+x] = (fixtoi(fatan(fdiv(itofix(x-320), itofix(y-200)))))<<9;
            else
                lookup[640*y+x] = (128)<<8;
            n = itofix(x-320);
            m = itofix(y-200);
            if ((int)sqrt((double)((x-320)*(x-320)+(y-200)*(y-200))) != 0)
            {
                if (deep)
                    lookup[640*y+x] += (4096)/(int)sqrt((x-320)*(x-320)+(y-200)*(y-200));
                else
                    lookup[640*y+x] += (int)sqrt((x-320)*(x-320)+(y-200)*(y-200));
            }
            if (orientation)
            {
                char temp;
                temp = lookup[640*y+x]/256;
                lookup[640*y+x] = lookup[640*y+x]*256+temp;
            }
        }
    }
}

// since there is asm code in the function, DO NOT INLINE
void DrawTunnel(BITMAP* buffer, unsigned short* lookup, int x, int y, int prof, int angle, TDeformTable * deform)
{
    int offset;
    int _x,_y;
    int i;

    // offset in map
    offset = (angle&255)*256+(prof&255);

    x = MID(0,160-x,319);
    y = MID(0,100-y,199);


    for (_y = 0;_y < 200;_y++)
    {
        unsigned char* pPicture = _image;
        unsigned short * pLookup;
        unsigned char * pDest;
        pLookup = lookup+(y+_y)*640+x;
        pDest = buffer->line[_y];
        for (_x = 0;_x < 320;_x++)
        {
            unsigned char pixel;
            unsigned short address;
            unsigned char save;
            unsigned int def;
            address = *pLookup++;
            save = address>>8;
            address+=offset;
            def=deform->table[save];
            address+=def;
            pixel = *(_map+(address&0xffff));
            *pDest++ = *pPicture++ + pixel;
        }
    }
    /*
      for(_y=0;_y<200;_y++)
      asm volatile ("
        pushl %%ebp
        movl %%ecx,%%ebp
        movl $320,%%ecx
        _tunnel_loop:
        xorl %%ebx,%%ebx
        movl (%%edx,%%ecx,2),%%eax
        movb %%ah,%%bl
        addl %%ebp,%%eax
        movl (%%esi,%%ebx,4),%%ebx
        addl %%ebx,%%eax
        andl $0xffff,%%eax
        movb __map(%%eax),%%al
        movb %%al,(%%edi,%%ecx)

        decl %%ecx
        jns  _tunnel_loop
        popl %%ebp
        "
        :
        : "c" (offset),"D" (buffer->line[_y]), "S" (&deform->table), "d" (lookup+(y+_y)*640+x)
        : "eax","ebx","ecx","edx","esi","edi"
        );
    */
    /*  // draw the tunnel on the buffer
      asm volatile ("
        pushl %%ebp
        movl %%ecx,%%ebp
        movl $320*200/1,%%ecx
        glenz_tunnel_loop:
        xorl %%ebx,%%ebx
        movl (%%edx,%%ecx,2),%%eax
        movb %%ah,%%bl    // ***
        addl %%ebp,%%eax
        movl (%%esi,%%ebx,4),%%ebx
        addl %%ebx,%%eax
        andl $0xffff,%%eax
        movb __map(%%eax),%%al
    //    movb __image(%%ecx),%%bl
    //    addb %%bl,%%al
        movb %%al,(%%edi,%%ecx)

        decl %%ecx
        jns  glenz_tunnel_loop
        popl %%ebp
        "
        :
      : "c" (offset),"D" (buffer->dat), "S" (&deform->table), "d" (lookup)
        : "eax","ebx","ecx","edx","esi","edi"
        );
    */
}

void DrawSimpleTunnel(BITMAP* buffer, unsigned short* lookup, int _x, int _y, int prof, int angle,BITMAP *map)
{
    int offset;
    int x,y;
    unsigned char * _map=map->dat;

    // offset in map
    offset = (angle&255)*256+(prof&255);

    _x = MID(0,160-_x,319);
    _y = MID(0,100-_y,199);

    for (y = 0;y < 200;y++)
    {
        unsigned char* pPicture = _image;
        unsigned short * pLookup;
        unsigned char * pDest;
        pLookup = lookup+(y+_y)*640+_x;
        pDest = buffer->line[y];
        for (x = 0;x < 320;x++)
        {

            unsigned char pixel;
            unsigned short address;
            unsigned char save;
            unsigned int def;
            address = *pLookup++;
            address+=offset;
            pixel = *(_map+(address&0xffff));
            *pDest++ =  pixel;

        }
    }

    /*
      for(_y=0;_y<200;_y++)
      asm volatile ("
        pushl %%ebp
        movl %%ecx,%%ebp
        movl $320,%%ecx
        __tunnel_loop:
        xorl %%ebx,%%ebx
        movl (%%edx,%%ecx,2),%%eax
        movb %%ah,%%bl
        addl %%ebp,%%eax
    //    movl (%%esi,%%ebx,4),%%ebx
    //    addl %%ebx,%%eax
        andl $0xffff,%%eax
        movb (%%eax,%%esi),%%al
        movb %%al,(%%edi,%%ecx)

        decl %%ecx
        jns  __tunnel_loop
        popl %%ebp
        "
        :
        : "c" (offset),"D" (buffer->line[_y]), "S" (map->dat), "d" (lookup+(y+_y)*640+x)
        : "eax","ebx","ecx","edx","esi","edi"
        );
    */
}


void DoDeformTable(TDeformTable* deform, int position)
{
    int i,c;

    // Compute deformation lookup
    for (i = 255;i >= 0;i--)
    {
        c = (
                fsin(((i+position)*deform->param[0].freq)+(deform->param[0].phase<<16))+
                fsin(((i+position)*deform->param[1].freq)+(deform->param[1].phase<<16))+
                fsin(((i+position)*deform->param[2].freq)+(deform->param[2].phase<<16))+
                fsin(((i+position)*deform->param[3].freq)+(deform->param[3].phase<<16)));

        // clipping
        deform->table[i] = ((128+(fixtoi(c*128)))*deform->intensity)/256;
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

    /*
    for(i=img->w*img->h-1;i>=0;i--)
    {
    if ((*h)<(unsigned char)threshold)
    *d=*s;
    else
    *d=0;
    d++;
    s++;
    h++;
    }
    */
    /*
    if (*((unsigned char*)height->dat+i) < threshold)
    *((unsigned char*)dest->dat+i)=*((unsigned char*)img->dat+i);
    else
    *((unsigned char*)dest->dat+i)=0;
    */

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


// WARNING : thresold is now SIGNED
//           -128 ==> Black
//              0 ==> Normal
//           +127 ==> White
// this new function is designed to be used with a luminosity merging...

void ThresholdSmoothMono(BITMAP *dest, BITMAP *height, int fakeThreshold, int specular, int min, int max)
{
    register int i,j;
    register unsigned char * d,*h;
    unsigned char table[256];
    int   threshold;
    float pente,level;

    // adjust specular
    specular = MID(1,specular,127);

    // adjust threshold for specular
    fakeThreshold = MID(-128,fakeThreshold,127);
    threshold = (-fakeThreshold * (abs(fakeThreshold)+specular*1.2)) /128;

    threshold = threshold+128;

    pente = (255.0-0.0) / (2.0*specular);

    if ((threshold-specular)<0)
        level = pente * abs( threshold-specular );
    else
        level = 0;

    for (i=0;i < 256;i++)
    {
        int v;

        if (i<threshold-specular)
            v = 0;
        else
            if (i>threshold+specular)
                v = 255;
            else
            {
                v = level;
                level += pente;
            }

        table[i] = MID(min,v,max);
    }


    for (j = dest->h;j > 0;j--)
    {
        unsigned char yop;
        h = (unsigned char*)height->line[j];
        d = (unsigned char*)dest->line[j];
        for (i = dest->w;i > 0;i--)
        {
            *d++ = table[*h++];
        }
    }

    /*
      h = (unsigned char*)height->dat;
      d = (unsigned char*)dest->dat;
      for(i = height->w*height->h-1;i >= 0;i--)
      {
        *d = table[*h];
        d++;
        h++;
      }
    */
    /*
      rectfill(dest,0,0,256,128,0);

      for(i=0;i < 256;i++)
      {
        vline(dest,i,0,table[i]/2,255);
      }
    */
}


void drawRotatedBack(BITMAP * buffer, BITMAP * back, fixed angle, int dist)
{
    V3D points[4];
    int s,z;
    int size = 255;

    s = dist;

    points[0].x = itofix(0);
    points[0].y = itofix(0);
    points[0].x = (s*fcos(angle))+itofix(160);
    points[0].y = (s*fsin(angle))+itofix(100);
    points[0].z = z;
    points[0].u = 0;
    points[0].v = 0;
    points[0].c = (itofix(127)+127*fsin(1*angle))>>16;;;

    points[1].x = itofix(320);
    points[1].y = itofix(0);
    points[1].x = (s*fcos(angle+itofix(64)))+itofix(128);
    points[1].y = (s*fsin(angle+itofix(64)))+itofix(64);
    points[1].z = z;
    points[1].u = size<<16;
    points[1].v = 0;
    points[1].c = (itofix(192)+63*fsin(2*angle))>>16;

    points[2].x = itofix(320);
    points[2].y = itofix(200);
    points[2].x = (s*fcos(angle+itofix(128)))+itofix(128);
    points[2].y = (s*fsin(angle+itofix(128)))+itofix(64);
    points[2].z = z;
    points[2].u = size<<16;
    points[2].v = size<<16;
    points[2].c = (itofix(127)+127*fsin(3*angle))>>16;;;

    points[3].x = itofix(0);
    points[3].y = itofix(200);
    points[3].x = (s*fcos(angle+itofix(192)))+itofix(128);
    points[3].y = (s*fsin(angle+itofix(192)))+itofix(64);
    points[3].z = z;
    points[3].u = 0;
    points[3].v = size<<16;
    points[3].c = (itofix(192)+63*fsin(angle/3))>>16;

    quad3d(buffer, POLYTYPE_ATEX, back, &points[0], &points[1], &points[2], &points[3]);
}

void drawRotatedBack2(BITMAP * buffer, BITMAP * back, fixed angle, int dist, int frame)
{
    V3D points[4];
    int s,z;
    int sizex = 255*1,sizey = 127*1;

    s = dist;
    frame = itofix(frame);

    points[0].x = itofix(0);
    points[0].y = itofix(0);
    points[0].x = (s*fcos(angle))+itofix(128);
    points[0].y = (s*fsin(angle))+itofix(64);
    points[0].z = z;
    points[0].u = 0;
    points[0].v = 0;
    points[0].c = (itofix(127)+127*fsin(1*frame))>>16;;;

    points[1].x = itofix(320);
    points[1].y = itofix(0);
    points[1].x = (s*fcos(angle+itofix(64)))+itofix(128);
    points[1].y = (s*fsin(angle+itofix(64)))+itofix(64);
    points[1].z = z;
    points[1].u = sizex<<16;
    points[1].v = 0;
    points[1].c = (itofix(192)+63*fsin(2*frame))>>16;

    points[2].x = itofix(320);
    points[2].y = itofix(200);
    points[2].x = (s*fcos(angle+itofix(128)))+itofix(128);
    points[2].y = (s*fsin(angle+itofix(128)))+itofix(64);
    points[2].z = z;
    points[2].u = sizex<<16;
    points[2].v = sizey<<16;
    points[2].c = (itofix(127)+127*fsin(3*frame))>>16;;;

    points[3].x = itofix(0);
    points[3].y = itofix(200);
    points[3].x = (s*fcos(angle+itofix(192)))+itofix(128);
    points[3].y = (s*fsin(angle+itofix(192)))+itofix(64);
    points[3].z = z;
    points[3].u = 0;
    points[3].v = sizey<<16;
    points[3].c = (itofix(192)+63*fsin(frame/3))>>16;

    quad3d(buffer, POLYTYPE_ATEX_LIT, back, &points[0], &points[1], &points[2], &points[3]);
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
            _putpixel(dest,x,y,_getpixel(src,ix+x+_getpixel(deform,dx+x,dy+y),y+iy));
}

/*
void DeformBitmapScroll(BITMAP *dest, BITMAP *src, BITMAP *deform, int dx, int dy, int ix, int iy)
{
  int y;

  for(y = 0;y < dest->h;y++)
  {
    asm("
      pushl %%ebp
      movl  %%eax,%%ebp
      xorl  %%ebx,%%ebx

      DeformBitmapScroll_Loop:
      movb (%%edx),%%bl
      movl %%esi,%%ecx
      addl %%ebx,%%ecx
      movb (%%ecx),%%al
      movb %%al,(%%edi)

      addl $1,%%edi
      addl $1,%%edx
      addl $1,%%esi
      decl %%ebp
      jnz  DeformBitmapScroll_Loop

      popl  %%ebp
      "
      :
    : "a" (dest->w), "d" (deform->line[y+dy]+dx), "D" (dest->line[y]), "S" (src->line[y+iy]+ix)
      : "eax", "ebx", "ecx", "edx", "edi", "esi"
      );
  }

}
*/




void DrawPlasma16(BITMAP * buffer, BITMAP * image, int frame, int amplitude, int deform)
{
    // image must be 450x330 !!!

    int i,y,x;

    unsigned long sinus[320+200];

    if (deform)
    {
        for (i = 0;i < 320;i++)
            sinus[i] = fixtoi(
                           itofix(64)+
                           ((amplitude*fsin(itofix((i+frame/1)&255))
                             +amplitude*fcos(itofix((frame/7)&255))
                             +amplitude*fsin(itofix((frame/2-i)&255))
                             +amplitude*fcos(itofix((frame/4+i/3)&255))
                            )/16))*450*2;

        for (i = 0;i < 200;i++)
            sinus[i+320] = 2*fixtoi(
                               itofix(64)+
                               ((amplitude*fsin(itofix((i+frame/9)&255))
                                 +amplitude*fcos(itofix((frame/3)&255))
                                 +amplitude*fsin(itofix((frame+i)&255))
                                 +amplitude*fcos(itofix((frame*2+i/7)&255))
                                )/16));
    }
    else
    {
        for (i = 0;i < 320;i++)
            sinus[i] = 2*fixtoi(
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
                                )/16))*450*2;
    }

    {
        unsigned char *dest,*img;

        dest = buffer->dat;
        img = image->dat;

        for (y = 199;y >= 0;y--)
        {

            unsigned short *D,*S;
            D=dest+320*y;
            S=img+450*y;
            for (x=0;x<320;x++)
            {
                *D=*(S+sinus[x]+sinus[320+y]);
                S++;
                D++;
            }

            /*
                asm volatile ("
                  _Xloop:
                  movl    (%%edx),%%eax
                  addl    %%ebx,%%eax
                  movw    (%%esi,%%eax),%%ax
                  addl    $2,%%esi
                  // ----

                  movw    %%ax,(%%edi)
                  addl    $2,%%edi
                  addl    $4,%%edx
                  decl    %%ecx
                  jns     _Xloop
                  "
                  :
                  : "D" (dest+640*y), "S" (img+2*450*y), "d" (sinus), "c" (319), "b" (sinus[320+y])
                  : "eax","ebx","ecx","edx","edi","esi");
            */
        }

    }
}

void DrawPlasma(BITMAP * buffer, BITMAP * image, int frame, int amplitude, int deform)
{
    // image must be 450x330 !!!

    int i,y,x;

    unsigned long sinus[320+200];

    if (deform)
    {
        for (i = 0;i < 320;i++)
            sinus[i] = fixtoi(
                           itofix(64)+
                           ((amplitude*fsin(itofix((i+frame/1)&255))
                             +amplitude*fcos(itofix((frame/7)&255))
                             +amplitude*fsin(itofix((frame/2-i)&255))
                             +amplitude*fcos(itofix((frame/4+i/3)&255))
                            )/16))*450;

        for (i = 0;i < 200;i++)
            sinus[i+320] = fixtoi(
                               itofix(64)+
                               ((amplitude*fsin(itofix((i+frame/9)&255))
                                 +amplitude*fcos(itofix((frame/3)&255))
                                 +amplitude*fsin(itofix((frame+i)&255))
                                 +amplitude*fcos(itofix((frame*2+i/7)&255))
                                )/16));

        for (i = 0;i < 320;i++)
            sinus[i] = fixtoi(itofix(64)+amplitude*
                              (fsin(itofix((frame*7)&255))+fcos(itofix((i-frame/2)&255)))
                              /8)*450;

        for (i = 0;i < 200;i++)
            sinus[i+320] = fixtoi(itofix(64)+amplitude*
                                  (fcos(itofix((frame*3)&255))+fcos(itofix((i+frame)&255)))
                                  /8);
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

        dest = buffer->dat;
        img = image->dat;

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
            /*
                asm volatile ("
                  Xloop:
                  movl    (%%edx),%%eax
                  addl    %%ebx,%%eax
                  movb    (%%esi,%%eax),%%al
                  incl    %%esi
                  // ----
            //      shrb    $1,%%al
                  subb    $1,%%al;

                  movb    %%al,(%%edi)
                  incl    %%edi
                  addl    $4,%%edx
                  decl    %%ecx
                  jns     Xloop
                  "
                  :
                  : "D" (dest+320*y), "S" (img+450*y), "d" (sinus), "c" (319), "b" (sinus[320+y])
                  : "eax","ebx","ecx","edx","edi","esi");
            */


        }
    }
}

void InitBumpTable(struct TBumpTable * table,int specular)
{
    int y;

    table->lightMap=create_bitmap_ex(8,256,256);
    table->deltaxy =NULL;


    for (y = 0;y < 256;y++)
    {
        int color;

        color = 255-(y*y/16);
        color = MID(128,color,255);
        circle(table->lightMap,127,127,y,color);
        circle(table->lightMap,128,127,y,color);
        circle(table->lightMap,127,128,y,color);
    }
}

void SetBumpTable(struct TBumpTable * table, BITMAP * map)
{
    int x,y;

    if (table->deltaxy!=NULL)
        destroy_bitmap(table->deltaxy);

    table->deltaxy =create_bitmap_ex(8,map->w*2,map->h);
    clear(table->deltaxy);

    for (y = 1;y < map->h-2; y++)
        for (x = 1;x < map->w-2; x++)
        {
            char i,j;
            i = (*(map->line[y]+x-1)-*(map->line[y]+x+1))/2;
            j = (*(map->line[y-1]+x)-*(map->line[y+1]+x))/2;
            i = MID(-64,i,64);
            j = MID(-64,j,64);
            *((char*)table->deltaxy->line[y]+2*x)   = i;
            *((char*)table->deltaxy->line[y]+2*x+1) = j;
        }
}

void DrawBump(BITMAP * lum, int _x,int _y, struct TBumpTable * table)
{
    int x1,y1,x2,y2,vx,vy,sx,sy,tx,ty,dx2,dy2,dx1,dy1,cx,cy,y;

    x1 = _x-128;
    y1 = _y-128;
    x2 = x1+255;
    y2 = y1+255;
    tx = x1;
    ty = y1;

    if (
        (x1>lum->w)||
        (y1>lum->h)||
        (x2<0)||
        (y2<0)
    ) return;
    /*
        rectfill(lum, -1, -1, x1, y2, 128);
        rectfill(lum, -1, y2, x2-1, 200, 128);
        rectfill(lum, x1, -1, 320, y1, 128);
        rectfill(lum, x2-1, y1, 320, 200, 128);
    */
    sx = lum->cr - x1;
    sx = MID(0,sx,255);
    dx2 = x1+sx;

    sy = lum->cb - y1;
    sy = MID(0,sy,255);
    dy2 = y1+sy;

    dx1=x1;
    dy1=y1;
    cx=0;
    cy=0;
    if (x1<lum->cl)
    {
        cx=lum->cl-x1;
        x1+=cx;
        sx-=cx;
    }
    if (y1<lum->ct)
    {
        cy=lum->ct-y1;
        y1+=cy;
        sy-=cy;
    }

    for (y = sy-1;y!=0 ;y--)
    {
        int x;
        char * src,*dest;
        vx = (x1)-tx;
        vy = (y+y1)-ty;
        src = (char*) (table->deltaxy->line[y+y1]+2*(x1));
        dest = lum->line[y+y1]+x1; //++++++++++++
        /*
              asm volatile ("
                push %%ebp
                movl %%eax,%%ebp
                xorl %%ebx,%%ebx

                bump_loop:
                  movb  (%%esi),%%bh
                  movb  1(%%esi),%%bl
                  addb  %%ch,%%bh
                  addb  %%cl,%%bl
                  movb  (%%edx,%%ebx),%%al
                  incb  %%ch

                  addl  $2,%%esi
                  movb  %%al,(%%edi)
                  incl  %%edi

                  dec %%ebp
                jnz bump_loop

                pop  %%ebp
              "
              :
              : "D"(dest),"S"(src),"d"(table->lightMap->dat),"c"(vx*256+vy),"a"(sx-1)
              : "eax","ebx","ecx","edx","esi","edi"
              );
        */

        for (x = sx-1;x!=0;x--)
        {
            register char dx,dy,c;

            dx = *(src++);
            dy = *(src++);
            c = (char) *(table->lightMap->line[(dy+vy)&255]+((dx+(vx++))&255));
            *(dest++) = c;
        }

    }
}




void DrawBumpEx(BITMAP * lum, int _x,int _y, struct TBumpTable * table, int offx, int offy)
{
    int x1,y1,x2,y2,vx,vy,sx,sy,tx,ty,dx2,dy2,dx1,dy1,cx,cy,y;

    x1 = _x-128;
    y1 = _y-128;
    x2 = x1+255;
    y2 = y1+255;
    tx = x1;
    ty = y1;

    if (
        (x1>lum->w)||
        (y1>lum->h)||
        (x2<0)||
        (y2<0)
    ) return;
    /*
        rectfill(lum, -1, -1, x1, y2, 128);
        rectfill(lum, -1, y2, x2, 200, 128);
        rectfill(lum, x1, -1, 320, y1, 128);
        rectfill(lum, x2, y1, 320, 200, 128);
    */
    sx = lum->cr - x1;
    sx = MID(0,sx,255);
    dx2 = x1+sx;

    sy = lum->cb - y1;
    sy = MID(0,sy,255);
    dy2 = y1+sy;

    dx1=x1;
    dy1=y1;
    cx=0;
    cy=0;
    if (x1<lum->cl)
    {
        cx=lum->cl-x1;
        x1+=cx;
        sx-=cx;
    }
    if (y1<lum->ct)
    {
        cy=lum->ct-y1;
        y1+=cy;
        sy-=cy;
    }

    for (y = sy-1;y!=0 ;y--)
    {
        int x;
        char * src,*dest;
        vx = (x1)-tx;
        vy = (y+y1)-ty;
        src = (char*) (table->deltaxy->line[y+offy]+2*(offx));
        dest = lum->line[y+y1]+x1; //++++++++++++
        /*
              asm volatile ("
                push %%ebp
                movl %%eax,%%ebp
                xorl %%ebx,%%ebx

                bump_loop_ex:
                  movb  (%%esi),%%bh
                  movb  1(%%esi),%%bl
                  addb  %%ch,%%bh
                  addb  %%cl,%%bl
                  movb  (%%edx,%%ebx),%%al
                  incb  %%ch

                  addl  $2,%%esi
                  movb  %%al,(%%edi)
                  incl  %%edi

                  dec %%ebp
                jnz bump_loop_ex

                pop  %%ebp
              "
              :
              : "D"(dest),"S"(src),"d"(table->lightMap->dat),"c"(vx*256+vy),"a"(sx-1)
              : "eax","ebx","ecx","edx","esi","edi"
              );
        */

        for (x = sx-1;x!=0;x--)
        {
            register char dx,dy,c;

            dx = *(src++);
            dy = *(src++);
            c = (char) *(table->lightMap->line[(dy+vy)&255]+((dx+(vx++))&255));
            *(dest++) = c;
        }

    }
}

