/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  05/98

  (c) KNIGHTS 1998

  Effects header

*/

#ifndef effects
#define effects

#include <allegro.h>
#include <math.h>
#include "k3d.h"


typedef struct TDeformTableParam
{
   int amplitude;
   int freq;
   int phase;
}TDeformTableParam;

typedef struct TDeformTable
{
   TDeformTableParam param[4];
   int intensity;
   int table[256];
}TDeformTable;


void SetTunnelMap(BITMAP * bmp);
void CalcTunnelLookup( unsigned short* lookup, int orientation, int deep);
void DrawTunnel(BITMAP* buffer, unsigned short* lookup,int x, int y,int prof, int angle, TDeformTable * deform);
void DrawSimpleTunnel(BITMAP* buffer, unsigned short* lookup,int x, int y,int prof, int angle, BITMAP *map);

void DoDeformTable(TDeformTable* deform, int position);
void Threshold(BITMAP *dest,BITMAP *img,BITMAP *height,int threshold);
void ThresholdSmooth(BITMAP *dest,BITMAP *img,BITMAP *height,int threshold);
void ThresholdSmoothMono(BITMAP *dest,BITMAP *height,int threshold, int specular, int min , int max);

 void drawRotatedBack(BITMAP * buffer,BITMAP * back,fixed angle,int dist);
 void drawRotatedBack2(BITMAP * buffer,BITMAP * back,fixed angle,int dist, int frame);

BITMAP * CreateDeformBitmap(BITMAP *img);
void DeformBitmap(BITMAP *dest,BITMAP *src,BITMAP *deform);
void DeformBitmapScroll(BITMAP *dest,BITMAP *src,BITMAP *deform,int dx,int dy,int ix,int iy);
void DrawPlasma(BITMAP * buffer,BITMAP * image,int frame,int amplitude,int deform);
void DrawPlasma16(BITMAP * buffer, BITMAP * image, int frame, int amplitude, int deform);

struct TBumpTable
{
  BITMAP * lightMap;
  BITMAP * deltaxy;
};

void InitBumpTable(struct TBumpTable * _table,int specular);
void SetBumpTable(struct TBumpTable * _table, BITMAP * map);
void DrawBump(BITMAP * buffer, int _x,int _y, struct TBumpTable * _table);
void DrawBumpEx(BITMAP * buffer, int _x,int _y, struct TBumpTable * _table, int dx, int dy);



#endif
