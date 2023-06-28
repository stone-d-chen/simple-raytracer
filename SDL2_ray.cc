#include <stdio.h>
#include "include/SDL.h"
#include "math.h"

#include "ray.cc"


int main(int ArgC, char **Args)
{
    bool is_running = false;
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {is_running = true; printf("Render\n");}

    image_u32 Image = AllocateImage(1280, 720);
    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Image.Width, Image.Height, SDL_WINDOW_POPUP_MENU);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Image.Width, Image.Height);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


#include "WorldInit.h" //just to get it out of the way


    f32 FilmDist = 1.0;
    f32 FilmW = 1.0;
    f32 FilmH = 1.0;
    if (Image.Width > Image.Height)
    {
        FilmH *= (f32)Image.Height / Image.Width;
    }
    if (Image.Width < Image.Height)
    {
        FilmW *= (f32)Image.Width / Image.Height;
    }
    f32 HalfFilmW = 0.5 * FilmW;
    f32 HalfFilmH = 0.5 * FilmH;

    f32 PixH = 1.0 / (f32)Image.Height;
    f32 PixW = 1.0 / (f32)Image.Width;
    f32 HalfPixH = 0.5 * PixH;
    f32 HalfPixW = 0.5 * PixW;


    v3f CameraP = v3f{ 0, -10, 1 };
    v3f LookAt = v3f{ 0, 0, 0, };
    v3f CameraZ = Normalize(CameraP - LookAt);
    v3f CameraX = Normalize(Cross(v3f{ 0,0,1 }, CameraZ)); //right hand rule
    v3f CameraY = Normalize(Cross(CameraZ, CameraX)); //right hand rule

    u32 RaysPerPixel = 4;


    while(is_running == true)
    {

        v3f RayOrigin = CameraP;
        u32 *Out = Image.Pixels;
        for(u32 Y = 0; Y < Image.Height; ++Y)
        {
            f32 FilmY = -1.0 + 2.0 * Y / (f32) Image.Height;
            for(u32 X = 0; X < Image.Width; ++X)
            {
                f32 FilmX = -1.0 + 2.0 * X / (f32) Image.Width;
                v3f color = {};
                for(u32 RayIndex = 0; RayIndex < RaysPerPixel; ++RayIndex)
                {
                    f32 OffY = (FilmY + HalfPixH) + HalfPixH * RandomBilateral(World.State);
                    f32 OffX = (FilmX + HalfPixW) + HalfPixW * RandomBilateral(World.State);
                    v3f RayDirection = Normalize(-CameraZ * FilmDist + CameraY * OffY * HalfFilmH + CameraX * OffX * HalfFilmW);
                    color += Raycast(World, Image, RayOrigin, RayDirection);
                }
                *Out++ = ARGBPack(color/RaysPerPixel);
            }
        }



        void *TextureMemory = malloc(Image.Width * Image.Height * sizeof(u32));
        s32 Pitch = Image.Width * sizeof(u32);

        SDL_LockTexture(texture, NULL, &TextureMemory, &Pitch);
        memcpy(TextureMemory, (void*)Image.Pixels, Image.Width*Image.Height*sizeof(u32));

        SDL_UnlockTexture(texture);
        SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(renderer);
        // SDL_RenderClear(renderer);
    }
    return(0);

}