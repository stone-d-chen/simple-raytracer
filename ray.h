#ifndef RAY_H
#define RAY_H
#include "math.h"

typedef struct
{
    u32 Width;
    u32 Height;
    u32 *Pixels;
} image_u32; 


typedef struct
{
    v3f EmitColor;
    v3f RefColor;
    f32 Specularity;
} material;

typedef struct
{
    v3f P;
    f32 r;
    u32 MatIndex;
} sphere;

typedef struct
{
    v3f N;
    f32 d;
    u32 MatIndex;
} plane;

typedef struct
{
    u32 MaterialCount;
    material *Materials;

    u32 SphereCount;
    sphere *Spheres;

    u32 PlaneCount;
    plane *Planes;

    random_series *State;

} world;


#endif