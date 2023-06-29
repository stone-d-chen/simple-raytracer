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

typedef struct
{
    world World;     // pointers to arrays
    image_u32 Image; // contains pointers to pixels

    u32 TileMinX;
    u32 TileMinY;
    u32 TileOnePastMaxX;
    u32 TileOnePastMaxY;

} work_order;

typedef struct
{
    u32 WorkOrderCount;
    work_order *Orders;

    volatile u32 NextWorkOrder;
    volatile u32 TilesRetired;
} work_queue;

u32 AddAndReturnPreviousValue(volatile u32 *Addend, u32 Value);



#endif