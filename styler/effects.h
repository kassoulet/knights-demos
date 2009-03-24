/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  02/98

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
   int multiplier;
   int table[256];
}TDeformTable;


void SetTunnelMap(BITMAP * bmp,BITMAP * image);
void CalcTunnelLookup( unsigned short* lookup, int orientation, int deep);
void DrawTunnel(BITMAP* buffer, unsigned short* lookup,int x, int y, TDeformTable * deform);

void DoDeformTable(TDeformTable* deform, int position);
void Threshold(BITMAP *dest,BITMAP *img,BITMAP *height,int threshold);
void ThresholdSmooth(BITMAP *dest,BITMAP *img,BITMAP *height,int threshold);
void InitAim(K3DTObject *aim);
 void drawRotatedBack(BITMAP * buffer,BITMAP * back,fixed angle);
BITMAP * CreateDeformBitmap(BITMAP *img);
void DeformBitmap(BITMAP *dest,BITMAP *src,BITMAP *deform);
void DeformBitmapScroll(BITMAP *dest,BITMAP *src,BITMAP *deform,int dx,int dy,int ix,int iy);
void DrawPlasma(BITMAP * buffer,BITMAP * image,int frame,int amplitude,int deform);
void DrawWater(BITMAP * buffer, BITMAP * old);
void CalcWater(void);

void CalcBump(BITMAP * image);
void DrawBump(BITMAP * dest,int x1, int y1);

#endif
