#ifndef MATH_H
#define MATH_H

#define ArrayCount(x) (sizeof(x) / sizeof(x[0]))

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
    f32 v[3];
};

v3f operator-(v3f a) {
    v3f r = {-a.x, -a.y, -a.z};
    return(r);
};
v3f operator+(v3f a, v3f b) {
    v3f r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return(r);
};

v3f operator-(v3f a, v3f b) {
    v3f r = a + -b;
    return(r);
};


v3f operator*(v3f a, f32 b) {
    v3f r = {a.x * b, a.y * b, a.z * b};
    return(r);
}
v3f operator*(f32 a, v3f b) {
    v3f r = b * a;
    return(r);
}
v3f operator/(v3f a, f32 b) {
    v3f r = a * (1.0 / b);
    return(r);
}

// special

f32 Inner(v3f a, v3f b)
{
    f32 r = a.x*a.x + a.y*b.y + a.z*b.z;
    return(r); 
}

v3f Normalize(v3f a)
{
    f32 norm = sqrtf(Inner(a, a));
    v3f r = a*(1.0/norm);
    return(r);
}

v3f Cross(v3f a, v3f b) {
    v3f r = {a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x};
    return(r);
};

#endif