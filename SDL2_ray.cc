#include <stdio.h>
#include "include/SDL.h"
#include "math.h"

#include "ray.cc"


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



int main(int ArgC, char **Args)
{
    bool is_running = false;
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {is_running = true; printf("Render\n");}

    image_u32 Image = AllocateImage(1280, 720);
    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Image.Width, Image.Height, SDL_WINDOW_POPUP_MENU);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Image.Width, Image.Height);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    world World = {};
    material Materials[] = 
    {
        {{0.3, 0.4, 0.5}, {0.0, 0.0, 0.8} },
        {{0.0, 0.0, 0.0}, {0.8, 0.6, 0.7} },
        {{0.0, 0.0, 0.0}, {0.0, 0.9, 0.3} },  
    };
    plane Planes[] = 
    {
        {{0,0,1}, 0, 1},
    };
    sphere Spheres[] = 
    {
        {{0,0,0}, 1, 2},
    };
    World.MaterialCount = ArrayCount(Materials);
    World.SphereCount = ArrayCount(Spheres);
    World.PlaneCount = ArrayCount(Planes);
    World.Materials = Materials;
    World.Spheres = Spheres;
    World.Planes = Planes;

    v3f CameraP = v3f{0, -10, 1};
    v3f LookAt  = v3f{0, 0, 0,};
    v3f CameraZ = Normalize(CameraP - LookAt);
    v3f CameraX = Normalize(Cross(v3f{0,0,1}, CameraZ)); //right hand rule
    v3f CameraY = Normalize(Cross(CameraZ, CameraX)); //right hand rule

    f32 FilmDist = 1.0;
    f32 FilmW = 1.0;
    f32 FilmH = 1.0;
    if(Image.Width > Image.Height)
    {
        FilmH *= (f32) Image.Height / Image.Width;
    }
    if(Image.Width < Image.Height)
    {
        FilmW *= (f32) Image.Width / Image.Height;
    }
    f32 HalfFilmW = 0.5 * FilmW;
    f32 HalfFilmH = 0.5 * FilmH;





    while(is_running == true)
    {
        u32 *Out = Image.Pixels;
        for(u32 Y = 0; Y < Image.Height; ++Y)
        {
            f32 FilmY = -1.0 + 2.0 * Y / (f32) Image.Height;
            for(u32 X = 0; X < Image.Width; ++X)
            {
                f32 FilmX = -1.0 + 2.0 * X / (f32) Image.Width;
                v3f RayOrigin = CameraP;
                v3f RayDirection = Normalize(-CameraZ * FilmDist + CameraY * FilmY * HalfFilmH + CameraX * FilmX * HalfFilmW);
                v3f color = Raycast(World, Image, RayOrigin, RayDirection);


                *Out++ = ARGBPack(color);
            }
        }




        SDL_UpdateTexture(texture, NULL, Image.Pixels, (s32) Image.Width * sizeof(u32));
        // TODO: Look into SDL_LockTexture
        SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }
    return(0);

}