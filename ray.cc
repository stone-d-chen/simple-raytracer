#include "ray.h"
#include "math.h"


u32 ARGBPack(v3f rgb)
{
    u32 r = ((u32) (255 * rgb.r)) << 16 |
            ((u32) (255 * rgb.g)) << 8 |
            ((u32) (255 * rgb.b)) << 0  |
            0xFF000000;
    return(r);
}

u32 ARGBPack(u32 A, u32 R, u32 G, u32 B)
{
    u32 r = (A << 24 | R << 16 | G << 8 | B << 0);
    return(r);
}

v3f Raycast(world *World, image_u32 Image, v3f RayOrigin, v3f RayDirection)
{
    f32 Tolerance = 0.0001;

    v3f Color = {0,0,0};
    v3f Attenuation = {1,1,1};

    v3f NextOrigin = {};
    v3f NextNormal = {};

    u32 MaxBounces = 8;
    for(u32 BounceCount = 0; BounceCount < MaxBounces; ++BounceCount)
    {
        f32 HitDistance = F32_MAX;
        u32 HitMatIndex = 0;

        for(u32 SphereIdx = 0; SphereIdx < World->SphereCount; ++SphereIdx)
        {
            sphere Sphere = World->Spheres[SphereIdx];
            
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
                    NextOrigin = t*RayDirection + RayOrigin;
                    NextNormal = Normalize(NextOrigin - Sphere.P);
                }
            }
        }
        
        for(u32 PlaneIdx = 0; PlaneIdx < World->PlaneCount; ++PlaneIdx)
        {
            plane Plane = World->Planes[PlaneIdx];
            f32 Denom = Inner(RayDirection, Plane.N);
            if ((Denom < -Tolerance) || (Denom > Tolerance))
            {
                f32 t = -(Inner(RayOrigin, Plane.N) + Plane.d) / Denom;
                if (t < HitDistance && t > Tolerance)
                {
                    HitDistance = t;
                    HitMatIndex = Plane.MatIndex;
                    NextOrigin = t*RayDirection + RayOrigin;
                    NextNormal = Normalize(Plane.N);
                }
            }
        }

        material Material = World->Materials[HitMatIndex];
        Color += Hadamard(Attenuation, Material.EmitColor);
        if(HitMatIndex != 0)
        {
            RayOrigin = NextOrigin;
            Attenuation = Hadamard(Attenuation, Material.RefColor);
            v3f PureBounce = Normalize(RayDirection - 2.0 * NextNormal * Inner(RayDirection, NextNormal));
            v3f RandomBounce = Normalize(NextNormal +           v3f{RandomBilateral(&(World->State)),
                                                                    RandomBilateral(&(World->State)),
                                                                    RandomBilateral(&(World->State))});
            RayDirection = Normalize(PureBounce * Material.Specularity + RandomBounce * (1 - Material.Specularity));
        }
        else
        {
            break;
        }

    }
    return(Color);
}

u32 *GetPixelPointer(image_u32 Image, u32 Y, u32 X)
{
    u32 *Out = Image.Pixels + Image.Width*Y + X;
    return(Out);
}

void RenderTile(world World, image_u32 Image,
                u32 MinX, u32 MinY, u32 OnePastMaxX, u32 OnePastMaxY)
{
    u32 RaysPerPixel = 1;

    f32 FilmDist = 1.0;
    f32 FilmW = 1.0;
    f32 FilmH = 1.0;
    if (Image.Width > Image.Height) FilmH *= (f32)Image.Height / Image.Width;
    if (Image.Width < Image.Height) FilmW *= (f32)Image.Width / Image.Height;

    f32 HalfFilmW = 0.5 * FilmW;
    f32 HalfFilmH = 0.5 * FilmH;

    f32 PixH = 2.0 / (f32)Image.Height;
    f32 PixW = 2.0 / (f32)Image.Width;
    f32 HalfPixH = 0.5 * PixH;
    f32 HalfPixW = 0.5 * PixW;

    v3f CameraP = World.Camera.P;      
    v3f LookAt  = World.Camera.LookAt; 
    v3f CameraZ = World.Camera.Z;      
    v3f CameraX = World.Camera.X;      
    v3f CameraY = World.Camera.Y;      
    v3f RayOrigin = CameraP;

    u32 *PixelOut = Image.Pixels;

    for(u32 Y = MinY; Y < OnePastMaxY; ++Y)
    {
        PixelOut = GetPixelPointer(Image, Y, MinX);
        f32 FilmY = -1.0 + 2.0 * Y / (f32) Image.Height;
        for(u32 X = MinX; X < OnePastMaxX; ++X)
        {
            f32 FilmX = -1.0 + 2.0 * X / (f32) Image.Width;

            v3f color = {};
            for(u32 RayIndex = 0; RayIndex < RaysPerPixel; ++RayIndex)
            {
                f32 OffY = (FilmY + HalfPixH) + HalfPixH * RandomBilateral(&World.State);
                f32 OffX = (FilmX + HalfPixW) + HalfPixW * RandomBilateral(&World.State);
                v3f RayDirection = Normalize(-CameraZ * FilmDist + CameraY * OffY * HalfFilmH + CameraX * OffX * HalfFilmW);
                            
                color += Raycast(&World, Image, RayOrigin, RayDirection);   // M_k = M_{k-1} + (x_k - M_{k-1})/k
            }
            *PixelOut++ = ARGBPack(color/RaysPerPixel);

        }
    }
}


//this will be my threadproc
void RenderTile(work_queue *Queue)
{
    while(Queue->TilesRetired < Queue->WorkOrderCount)
    {
        u32 WorkOrderIndex = AddAndReturnPreviousValue(&Queue->NextWorkOrder, 1);
        if(WorkOrderIndex >= Queue->WorkOrderCount) return;
        
        work_order *Order = Queue->Orders + WorkOrderIndex;

        RenderTile(Order->World,
                    Order->Image,
                    Order->TileMinX,
                    Order->TileMinY,
                    Order->TileOnePastMaxX,
                    Order->TileOnePastMaxY);

        AddAndReturnPreviousValue(&Queue->TilesRetired, 1);
    }
}
