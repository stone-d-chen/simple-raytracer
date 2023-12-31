#ifndef RAY_H
#define RAY_H

#include "math.h"

typedef struct
{
    u32 Width;
    u32 Height;
    u32 BufferWidth;
    u32 BufferHeight;
    u32 *Pixels;

    u32 Contributions;
    v3f *V3FColorArray;
} image_u32;

typedef struct
{
    u32 Up;
    u32 Down;
    u32 Left;
    u32 Right;
    u32 W;
    u32 A;
    u32 S;
    u32 D;
} user_inputs;

typedef struct
{
    v3f EmitColor;
    v3f RefColor;
    f32 Specularity;
    f32 Eta;
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
    v3f P;
    v3f LookAt;
    v3f X;
    v3f Y;
    v3f Z;
} camera;

typedef struct
{
    u32 MaterialCount;
    material *Materials;

    u32 SphereCount;
    sphere *Spheres;
    bvh_node *SphereBVH;

    u32 PlaneCount;
    plane *Planes;

    camera Camera;

    u32 RaysPerPixel;

    random_series State;


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

// InterlockedExchangeAdd()
u32 AddAndReturnPreviousValue(volatile u32 *Addend, u32 Value);



#endif