#include "math.h"

typedef struct
{
    v3f bbMin;
    v3f bbMax;

    u32 LeftChild;
    u32 RightChild;

    u32 FirstPrim;
    u32 PrimCount;
} bvh_node;

