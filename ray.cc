#include "ray.h"
#include "math.h"



v3f Raycast(world World, image_u32 Image, v3f RayOrigin, v3f RayDirection)
{
    f32 Tolerance = 0.0001;
    f32 HitDistance = F32_MAX;
    u32 HitMatIndex = 0;

    for(u32 SphereIdx = 0; SphereIdx < World.SphereCount; ++SphereIdx)
    {
        sphere Sphere = World.Spheres[SphereIdx];
        
        // determine if we hit the sphere or not
        f32 a = Inner(RayDirection, RayDirection);
        v3f OffsetSphereOrigin = RayOrigin - Sphere.P;
        f32 b = 2.0 * Inner(RayDirection, OffsetSphereOrigin);
        f32 c = Inner(OffsetSphereOrigin, OffsetSphereOrigin) - Sphere.r * Sphere.r;
        f32 RootTerm = sqrtf(b * b - 4.0f * a * c);
        if (RootTerm > Tolerance)
        {
            f32 t =  (-b + RootTerm) / (2.0 * a);
            f32 tn = (-b - RootTerm) / (2.0 * a);
            if (tn > Tolerance)
            {
                t = tn;
            }
            if (t < HitDistance && t > Tolerance)
            {
                HitDistance = t;
                HitMatIndex = Sphere.MatIndex;
            }
        }
    }
    
    for(u32 PlaneIdx = 0; PlaneIdx < World.PlaneCount; ++PlaneIdx)
    {
        plane Plane = World.Planes[PlaneIdx];
        f32 Denom = Inner(RayDirection, Plane.N);
        if ((Denom < -Tolerance) || (Denom > Tolerance))
        {
            f32 t = -(Inner(RayOrigin, Plane.N) + Plane.d) / Denom;
            if (t < HitDistance && t > Tolerance)
            {
                HitDistance = t;
                HitMatIndex = Plane.MatIndex;
            }
        }
    }

    material Material = World.Materials[HitMatIndex];
    v3f Color = Material.RefColor;

    // v3f Color = v3f{0.8,0.5,0.3};
    return(Color);
}

u32 ARGBPack(v3f rgb)
{
    u32 r = (u32) (255 * rgb.r) << 24 |
            (u32) (255 * rgb.g) << 16 |
            (u32) (255 * rgb.b) << 0  |
            0xFF000000;
    return(r);
}

u32 ARGBPack(u32 A, u32 R, u32 G, u32 B)
{
    u32 r = (A << 24 | R << 16 | G << 8 | B << 0);
    return(r);
}

image_u32 AllocateImage(u32 Width, u32 Height)
{
    image_u32 Image = {};
    Image.Width = Width;
    Image.Height = Height;
    Image.Pixels = (u32 *)malloc(Width * Height * sizeof(u32));
    return(Image);
}