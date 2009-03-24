/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  02/98

  (c) KNIGHTS 1998

  Effects

*/


#include "effects.h"


char _map[256*256];
char _image[320*200];

void SetTunnelMap(BITMAP * bmp, BITMAP * img)
{
    int x,y;

    for (x = 0;x < 256;x++)
        for (y = 0;y < 256;y++)
            _map[y*256+x] = getpixel(bmp, x, y);
    for (x = 0;x < 320;x++)
        for (y = 0;y < 200;y++)
            _image[y*320+x] = getpixel(img, x, y);
}


void CalcTunnelLookup(unsigned short* lookup, int orientation, int deep)
{
    int x,y;

    for (x = 0;x < 320;x++)
    {
        for (y = 0;y < 200;y++)
        {
            fixed n,m;
            if ((y-100) != 0)
                lookup[320*y+x] = (fixtoi(fatan(fdiv(itofix(x-160), itofix(y-100)))))<<9;
            else
                lookup[320*y+x] = (128)<<8;
            n = itofix(x-160);
            m = itofix(y-100);
            if (sqrt((int)(x-160)*(x-160)+(y-100)*(y-100)) != 0)
            {
                if (deep)
                    lookup[320*y+x] += (4096)/(int)sqrt((x-160)*(x-160)+(y-100)*(y-100));
                else
                    lookup[320*y+x] += (int)sqrt((x-160)*(x-160)+(y-100)*(y-100));
            }
            if (orientation)
            {
                char temp;
                temp = lookup[320*y+x]/256;
                lookup[320*y+x] = lookup[320*y+x]*256+temp;
            }
        }
    }
}

// since there is asm code in the function, DO NOT INLINE
void DrawTunnel(BITMAP* buffer, unsigned short* lookup, int x, int y, TDeformTable * deform)
{
    int offset;
    int i;

    unsigned char* pPicture = _image;

    // offset in map
    offset = (x&255)*256+(y&255);
    for (i = 0;i < 320*200;i++)
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

    // draw the tunnel on the buffer
    /*
      // offset in map
      offset = (x&255)*256+(y&255);

      for(i = 0;i < 320*200;i++)
      {
      *(((char*)buffer->dat)+i) =
      *((char*)_image+((lookup[i]+deform->table[lookup[i]&255]+offset)&0xffff));
      }
      */
    /*
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
      movb __image(%%ecx),%%bl
      addb %%bl,%%al
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

    s = (itofix(420)+150*fsin(angle))>>16;

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
    points[1].x = (s*fcos(angle+itofix(64)))+itofix(160);
    points[1].y = (s*fsin(angle+itofix(64)))+itofix(100);
    points[1].z = z;
    points[1].u = size<<16;
    points[1].v = 0;
    points[1].c = (itofix(192)+63*fsin(2*angle))>>16;

    points[2].x = itofix(320);
    points[2].y = itofix(200);
    points[2].x = (s*fcos(angle+itofix(128)))+itofix(160);
    points[2].y = (s*fsin(angle+itofix(128)))+itofix(100);
    points[2].z = z;
    points[2].u = size<<16;
    points[2].v = size<<16;
    points[2].c = (itofix(127)+127*fsin(3*angle))>>16;;;

    points[3].x = itofix(0);
    points[3].y = itofix(200);
    points[3].x = (s*fcos(angle+itofix(192)))+itofix(160);
    points[3].y = (s*fsin(angle+itofix(192)))+itofix(100);
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

            /*
              asm volatile ("
                Xloop:
                movl    (%%edx),%%eax
                addl    %%ebx,%%eax
                movb    (%%esi,%%eax),%%al
                incl    %%esi
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
    /*
    asm volatile ("
    movl    $199,%%ebx
    Yloop:
    movl    $319,%%ecx
    Xloop:
    movl    (%%edx,%%ecx,4),%%eax
    addl    319*4(%%edx,%%ebx,4),%%eax
    //              andl    $0xffff,%%eax
    movb    (%%esi,%%eax),%%al
    incl    %%esi
    movb    %%al,(%%edi)
    incl    %%edi

    decl    %%ecx
    jns     Xloop
    addl    $130,%%esi     //80
    decl    %%ebx
    jnz     Yloop
    "
    :
    : "D" (buffer->dat), "S" (image->dat), "d" (sinus)
    : "eax","ebx","ecx","edx","edi","esi");
    */
}

/*



WARNING
*/

void DrawWater(BITMAP * buffer, BITMAP * old)
{
    // compute water data
    /*
    asm volatile("
      pushl %%ebp
      xorl %%ebx,%%ebx
      xorl %%ecx,%%ecx
      xorl %%edx,%%edx
      movl  $320*150,%%ebp

      water_loop:
      // pixel loop
      xorl %%eax,%%eax

      // add the S,W,E,N pixels
      movb -320(%%esi),%%al
      movb -1(%%esi),%%bl
      movb 1(%%esi),%%cl
      movb 320(%%esi),%%dl

      // add them and div by 2
      addl %%ebx,%%eax
      addl %%edx,%%ecx
      addl %%ecx,%%eax
      shrl $1,%%eax
      andl $0xff,%%eax
      movb (%%edi),%%ah

      // search into the lookup
      movb __map(%%eax),%%bl

      // plot it
      movb %%bl,(%%edi)

      incl %%edi
      incl %%esi
      dec %%ebp
      jnz water_loop

      popl %%ebp
      ":
    : "D"(buffer->dat+320*25), "S"(old->dat+320*25)
      : "eax","ebx","ecx","edx","esi","edi"
      );
    */
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


/*
dx = *(map->line[y]+x+1)-*(map->line[y]+x-1);
vx = x-mouse_x;
c = abs(vx-dx);
dy = *(map->line[y+1]+x)-*(map->line[y-1]+x);
vy = y-mouse_y;
c += abs(vy-dy);

c = 255-c;
c = -c;
if(c<=0) c = 1;
if(c>255) c = 255;

*(buffer->line[y]+x) = c;
*/

