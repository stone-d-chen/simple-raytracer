#ifndef MATH_H
#define MATH_H

#include <stdbool.h>

typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;


union v3f
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };
    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };

};

#endif