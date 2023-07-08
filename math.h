#ifndef MATH_H
#define MATH_H

#include <stdbool.h>
#include <math.h>

#define F32_MAX 3.402823466e+38F
#define ArrayCount(x) (sizeof(x) / sizeof(x[0]))

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

v3f operator+(v3f a, f32 b)
{
    v3f r = {a.x + b, a.y + b, a.z + b};
    return(r);
}
v3f operator-(v3f a, f32 b)
{
    v3f r = a + -b;
    return(r);
}

/* 
    Mathops
*/

f32 clamp_min(f32 val, f32 min) {
    if(val < min) val = (f32) min;
    return(val);
}

f32 clamp_max(f32 val, f32 max) {
    if(val > max) val = (f32) max;
    return(val);
}


v3f fminf(v3f a, v3f b)
{
    v3f r = {fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)};
    return(r);
}
v3f fmaxf(v3f a, v3f b)
{
    v3f r = {fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)};
    return(r);
}

v3f Clamp01(v3f a)
{
    v3f r;
    r.x = clamp_min(clamp_max(a.x, 1), 0);
    r.y = clamp_min(clamp_max(a.y, 1), 0);
    r.z = clamp_min(clamp_max(a.z, 1), 0);
    return(r);
}

// inplace

v3f operator+=(v3f &a, v3f b)
{
    a = a + b;
    return(a);
}


// special


f32 Inner(v3f a, v3f b)
{
    f32 r = a.x*b.x + a.y*b.y + a.z*b.z;
    return(r); 
}

v3f Hadamard(v3f a, v3f b)
{
    v3f r = {a.x * b.x, a.y * b.y, a.z * b.z};
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

/*
    Random Series
*/

#include <random>

typedef struct
{
    u32 states[5];
    u32 counter;
} random_series;

random_series CreateRandomSeries()
{
    random_series Series = {(u32)rand(),
                            (u32)rand(),
                            (u32)rand(),
                            (u32)rand(),
                            (u32)rand(),
                            (u32)rand()};
    return(Series);
}

u32 XorWow(random_series *state)
{
    /* https://en.wikipedia.org/wiki/Xorshift#xorwow */
    u32 t = state->states[4];

    u32 s = state->states[0];
    state->states[4] = state->states[3];
    state->states[3] = state->states[2];
    state->states[2] = state->states[1];
    state->states[1] = s;

    t ^= t >> 2;
    t ^= t << 1;
    t ^= s ^ (s << 4);
    state->states[0] = t;
    state->counter += 362437;
    t += state->counter;
    return(t);
}
u32 XorShiftU32(random_series *state)
{
    // https://en.wikipedia.org/wiki/Xorshift
    u32 x = state->states[0];
    x ^= (x << 13);
    x ^= (x >> 17);
    x ^= (x << 5 );
    state->states[0] = x;
    return(x);
}

f32 RandomUnilateral(random_series *state)
{
    f32 Result = (f32) XorWow(state)/ UINT32_MAX;
    return(Result);
}

f32 RandomBilateral(random_series *state)
{
    f32 Result = -1.0 + 2.0*RandomUnilateral(state);
    return(Result);
}
f32 RandomUnilateral()
{
    f32 Result = (f32) rand() / (f32) RAND_MAX;
    return(Result);
}

f32 RandomBilateral()
{
    f32 Result = -1.0 + 2.0*RandomUnilateral();
    return(Result);
}


#endif