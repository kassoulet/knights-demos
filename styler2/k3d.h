/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  02/98

  (c) KNIGHTS 1998

  3D Engine header

*/

#ifndef k3d
#define k3d

#include <allegro.h>
#include <math.h>

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
  int                         zmax;
} K3DTObject;

/* an array of objects */
typedef struct K3DTWorld
{
  int objects;
  struct K3DTObject *object;
  struct K3DTPlace place;
} K3DTWorld;


void K3DInit(void);
void K3DInitObject(struct K3DTObject * obj);
void K3DInitWorld(struct K3DTWorld * obj);
void K3DDeleteObject(struct K3DTObject * obj);
void K3DDeleteWorld(struct K3DTWorld * obj);
int  K3DExtractObjectASC(char * pointer, struct K3DTObject * obj);
int  K3DLoadObjectASC(char * file, struct K3DTObject * obj);
void K3DSetObjectRendering(struct K3DTObject *obj, int f, int s, int c);
void K3DSetObjectMap(struct K3DTObject *obj, BITMAP* map);
void K3DEnvMapObject(struct K3DTObject *obj);
void K3DPlaceObject(struct K3DTObject *obj, fixed x, fixed y, fixed z, fixed rx, fixed ry, fixed rz);
void K3DRotateObject(struct K3DTObject *obj);
void K3DProjectObject(struct K3DTObject *obj);
void K3DDrawObject(BITMAP * buffer, struct K3DTObject *obj);
void K3DRenderObject(BITMAP * buffer, struct K3DTObject *obj);
void K3DBackFaceObject(struct K3DTObject *obj);


void _triangle(BITMAP * bmp, int x1, int  y1, int  x2, int  y2, int x3, int y3, int color);

#endif
