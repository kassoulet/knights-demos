/*

  Styler

  Coded by Gautier "Impulse" PORTET

  01/98  02/98

  (c) KNIGHTS 1998

  3D Engine

*/

#include "k3d.h"
#include <stdio.h>
#include <string.h>

unsigned char faceOrder[1024],faceDepth[1024];

void _triangle(BITMAP * bmp, int x1, int  y1, int  x2, int  y2, int x3, int y3, int color)
{
    if (y3 < y2)
    {
        int tx,ty;
        tx = x3;
        ty = y3;
        x3 = x2;
        y3 = y2;
        x2 = tx;
        y2 = ty;
    }

    if (y2 < y1)
    {
        int tx,ty;
        tx = x1;
        ty = y1;
        x1 = x2;
        y1 = y2;
        x2 = tx;
        y2 = ty;
    }

    if (y3 < y2)
    {
        int tx,ty;
        tx = x3;
        ty = y3;
        x3 = x2;
        y3 = y2;
        x2 = tx;
        y2 = ty;
    }

    // Vertical Clipping
//  if(y3 > 199) y3 = 199;
    if (y1 < 0) y1 = 0;


    {
        // Y1 to Y2 //
        fixed dx1,px1,dx2,px2;
        int sizey;
        int y;

        // delta X
        dx1 = fdiv(itofix(x2-x1), itofix(y2-y1));
        px1 = itofix(x1);
        dx2 = fdiv(itofix(x3-x1), itofix(y3-y1));
        px2 = itofix(x1);

        // size to draw
        sizey = y2-y1;

        for (y = y1;y < y2;y++)
        {
            // one Hline
            hline(bmp, fixtoi(px1), y, fixtoi(px2), color);
            px1 += dx1;
            px2 += dx2;
        }

        // Y2 to Y3 //
        // delta X
        dx1 = fdiv(itofix(x3-x2), itofix(y3-y2));
        px1 = itofix(x2);

        // size to draw
        sizey = y2-y1;

        for (y = y2;y < y3;y++)
        {
            // one Hline
            hline(bmp, fixtoi(px1), y, fixtoi(px2), color);
            px1 += dx1;
            px2 += dx2;
        }
    }
}


void K3DInit(void)
{
}

void K3DInitObject(struct K3DTObject * obj)
{
    obj->scale = itofix(1);
    obj->vertexes = 0;
    obj->faces = 0;
    obj->zmax = 16;
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
    long   faces,vertexes;

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
        } while (strncmp(line, "Named object:", 13) != 0);
        LoadLine(f, line);
    } while (strncmp(line, "Tri-mesh", 8) != 0);

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

    } while (!fin);

    if (i != vertexes)
    {
        printf("Error while parsing vertexes (%d found,%ld expexted)", i, vertexes);
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

    for (i = 0;i < obj->vertexes;i++)
    {
        obj->projected[i].u = obj->rotated[i].x-obj->rotated[i].z;
        obj->projected[i].v = obj->rotated[i].y-obj->rotated[i].z;

        obj->projected[i].u = obj->rotated[i].x-obj->rotated[i].z;
        obj->projected[i].v = obj->rotated[i].y-obj->rotated[i].z;
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
    //  return (*((unsigned char*)f2))-(*((unsigned char*)f1));
}

int byteSort(char * array, char * dest, int size)
{
    static int lookup[256];
    static int lookup2[256];
    int i;

    for (i = 255;i >= 0;i--)
        lookup[i] = 0;
    //   for(i=255;i>=0;i--)
    //     lookup2[i]=0;
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
        faceDepth[i] = 4*fixtoi(obj->rotated[obj->face[i].v1].z+obj->rotated[obj->face[i].v2].z+obj->rotated[obj->face[i].v3].z)/4;
        obj->face[i].z = obj->rotated[obj->face[i].v1].z+obj->rotated[obj->face[i].v2].z+obj->rotated[obj->face[i].v3].z;
        //    obj->face[i].color = 255-(faceDepth[i]/2);
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
            obj->face[i].color = 127-(obj->face[i].z>>16)/obj->zmax;
            //16
            obj->projected[obj->face[i].v1].color =
                obj->projected[obj->face[i].v2].color =
                    obj->projected[obj->face[i].v3].color = obj->face[i].color;
        }
    }

    if (obj->rendering.sorting == QSORT)
    {
        qsort(obj->face, obj->faces, sizeof(K3DTFace), qsort_FaceCompare);
        //    memcpy(faceOrder,faceDepth,obj->faces);
        //    qsort(faceOrder,obj->faces,sizeof(char),qsort_FaceCompare);
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
        obj->projected[i].z = obj->rotated[i].z;
    }
}

void K3DDrawObject(BITMAP * buffer, struct K3DTObject *obj)
{
    int i,c;

    if (obj->rendering.filling == PLOT)
    {
        for (i = 0;i < obj->vertexes;i++)  /*(obj->projected[i].y>>15)*/
        {
            c = 120+(obj->projected[i].y>>16);
            putpixel(buffer, (obj->projected[i].x>>16), (obj->projected[i].y>>16), c);
            //(obj->projected[i].y>>16));
            putpixel(buffer, (obj->projected[i].x>>16)+1, (obj->projected[i].y>>16), c);
            //(obj->projected[i].y>>16));
            putpixel(buffer, (obj->projected[i].x>>16), (obj->projected[i].y>>16)+1, c);
            //(obj->projected[i].y>>16));
            putpixel(buffer, (obj->projected[i].x>>16)+1, (obj->projected[i].y>>16)+1, c);
            //(obj->projected[i].y>>16));
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
        case TEXTURE_MASK:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_ATEX_MASK,
                               obj->face[i].map,
                               (V3D*)&obj->projected[obj->face[i].v1],
                               (V3D*)&obj->projected[obj->face[i].v2],
                               (V3D*)&obj->projected[obj->face[i].v3]
                              );
            }
            break;
        case TEXTURE_SHADED:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_ATEX_LIT,
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
        case TEXTURE_MASK_CORRECTED:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_ATEX_MASK,
                               obj->face[i].map,
                               (V3D*)&obj->projected[obj->face[i].v1],
                               (V3D*)&obj->projected[obj->face[i].v2],
                               (V3D*)&obj->projected[obj->face[i].v3]
                              );
            }
            break;
        case TEXTURE_SHADED_CORRECTED:
            for (i = 0;i < obj->faces;i++)
            {
                if (obj->face[i].visible)
                    triangle3d(buffer,
                               POLYTYPE_ATEX_LIT,
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

void K3DBackFaceObject(struct K3DTObject *obj)
{
    int i;

    for (i = 0;i < obj->faces;i++)
    {
        if (fixtoi(polygon_z_normal(&obj->rotated[obj->face[i].v1],
                                    &obj->rotated[obj->face[i].v2],
                                    &obj->rotated[obj->face[i].v3])) > 0)
            obj->face[i].visible = 0 ;
    }
}
