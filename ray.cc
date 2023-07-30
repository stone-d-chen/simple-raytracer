#include "ray.h"
#include "math.h"


u32 ARGBPack(v3f rgb)
{
    u32 r = ((u32) (255 * sqrt(rgb.r))) << 16 |
            ((u32) (255 * sqrt(rgb.g))) << 8 |
            ((u32) (255 * sqrt(rgb.b))) << 0  |
            0xFF000000;
    return(r);
}

u32 ARGBPack(u32 A, u32 R, u32 G, u32 B)
{
    u32 r = (A << 24 | R << 16 | G << 8 | B << 0);
    return(r);
}

void ResetImage(image_u32 *Image)
{
    Image->Contributions = 1;
    memset(Image->V3FColorArray, 0, Image->Height * Image->Width * sizeof(v3f));
}

void UpdateWorldState(world* World, image_u32 *Image, user_inputs UserInputs)
{
    if (UserInputs.Up)
    {
        World->Camera.P.z += 0.06;
        ResetImage(Image);
    }
    if (UserInputs.Down)
    {
        World->Camera.P.z -= 0.06;
        ResetImage(Image);

    }
    if (UserInputs.Right)
    {
        World->Camera.P.x += 0.06;
        ResetImage(Image);

    }
    if (UserInputs.Left)
    {
        World->Camera.P.x -= 0.06;
        ResetImage(Image);

    }
    if (UserInputs.W)
    {
        World->Camera.P.y += 0.06;
        World->Camera.LookAt.y += 0.06;
        ResetImage(Image);

    }
    if (UserInputs.S)
    {
        World->Camera.P.y -= 0.06;
        World->Camera.LookAt.y -= 0.06;
        ResetImage(Image);

    }
    if (UserInputs.D)
    {
        World->Camera.P.x += 0.06;
        World->Camera.LookAt.x += 0.06;
        ResetImage(Image);

    }
    if (UserInputs.A)
    {
        World->Camera.P.x -= 0.06;
        World->Camera.LookAt.x -= 0.06;
        ResetImage(Image);

    }
}

f32 GetReflectance(f32 CosTheta, f32 EtaOverEtaP)
{
    f32 Result = (1 - EtaOverEtaP) / (1 + EtaOverEtaP);
    Result *= Result;
    Result = Result + (1 - Result)*pow((1-CosTheta), 5);
    return(Result);
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

        IntersectBVH(RayDirection, RayOrigin, 0, World->Spheres, HitDistance, HitMatIndex, NextOrigin, NextNormal);
        
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

            if(Material.Eta > 0)
            {
                f32 Eta = Material.Eta;
                f32 EtaOverEtaP = 1.0/Eta;
                if(Inner(RayDirection, NextNormal) > 0)
                {
                    NextNormal = -1.0*NextNormal;
                    EtaOverEtaP = Eta;
                }

                f32 CosTheta = Inner(-RayDirection, NextNormal);
                f32 SinTheta = sqrt(1.0 - CosTheta*CosTheta);

                if( (SinTheta * EtaOverEtaP > 1.0) || GetReflectance(CosTheta, EtaOverEtaP) > RandomUnilateral(&(World->State)))
                {
                    RayDirection = RayDirection - 2.0 * Inner(RayDirection, NextNormal) * NextNormal;
                }
                else
                {
                    v3f RefracPerp = EtaOverEtaP * (RayDirection + CosTheta*NextNormal);
                    v3f RefracPara = -sqrtf(fabs(1.0 - Inner(RefracPerp, RefracPerp))) * NextNormal;
                    RayDirection = Normalize(RefracPerp + RefracPara);
                }
            }
            else
            {
                v3f PureBounce = Normalize(RayDirection - 2.0 * NextNormal * Inner(RayDirection, NextNormal));
                v3f RandomBounce = Normalize(NextNormal +           v3f{RandomBilateral(&(World->State)),
                                                                        RandomBilateral(&(World->State)),
                                                                        RandomBilateral(&(World->State))});
                RayDirection = Normalize(PureBounce * Material.Specularity + RandomBounce * (1 - Material.Specularity));

            }

            
        }
        else
        {
            break;
        }

    }
    return(Clamp01(Color));
}

u32 *GetPixelPointer(image_u32 Image, u32 Y, u32 X)
{
    u32 *Out = Image.Pixels       + Image.BufferWidth*Y + X;
    return(Out);
}
v3f *GetV3FPointer(image_u32 Image, u32 Y, u32 X)
{
    v3f *Out = Image.V3FColorArray + Image.BufferWidth*Y + X;
    return(Out);
}

void RenderTile(world World, image_u32 Image,
                u32 MinX, u32 MinY, u32 OnePastMaxX, u32 OnePastMaxY)
{
    u32 RaysPerPixel = World.RaysPerPixel;

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

    u32 *PixelOut = Image.Pixels;
    v3f *ColorOut = Image.V3FColorArray;



    static f32 FocusDist = 1.2;
    // FocusDist += 0.01;

    for(u32 Y = MinY; Y < OnePastMaxY; ++Y)
    {
        PixelOut = GetPixelPointer(Image, Y, MinX);
        v3f *ColorOut = GetV3FPointer(Image, Y, MinX);
        f32 FilmY = -1.0 + 2.0 * Y / (f32) Image.Height;
        
        for(u32 X = MinX; X < OnePastMaxX; ++X)
        {
            f32 FilmX = -1.0 + 2.0 * X / (f32) Image.Width;

            for(u32 RayIndex = 0; RayIndex < RaysPerPixel; ++RayIndex)
            {
                f32 OffY = (FilmY + HalfPixH) + HalfPixH * RandomBilateral(&World.State);
                f32 OffX = (FilmX + HalfPixW) + HalfPixW * RandomBilateral(&World.State);
                
                v3f RayDirection = Normalize(-CameraZ * FilmDist + CameraY*OffY*HalfFilmH +  CameraX*OffX*HalfFilmW);
                f32 RadiusFactor = 1/150.0;
                // v3f RayOrigin = CameraP;
                v3f RayOrigin = CameraP + v3f{RandomBilateral(&World.State) * RadiusFactor,
                                              RandomBilateral(&World.State) * RadiusFactor,
                                             RandomBilateral(&World.State) * RadiusFactor};
                            
                *ColorOut += Raycast(&World, Image, RayOrigin, RayDirection);   // M_k = M_{k-1} + (x_k - M_{k-1})/k
            }
            v3f color = *ColorOut++;

            *PixelOut++ = ARGBPack(color/Image.Contributions);

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
