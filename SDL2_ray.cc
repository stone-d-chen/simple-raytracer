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


    v3f *ColorArray = (v3f *) malloc(Image.Height * Image.Width * sizeof(v3f));
    u32 TotalSamples = 0;
    while(is_running == true)
    {

        v3f RayOrigin = CameraP;
        ++TotalSamples;
        u32 *PixelOut = Image.Pixels;
        v3f *ColorArrayOut = ColorArray;
        for(u32 Y = 0; Y < Image.Height; ++Y)
        {
            f32 FilmY = -1.0 + 2.0 * Y / (f32) Image.Height;
            for(u32 X = 0; X < Image.Width; ++X)
            {
                f32 FilmX = -1.0 + 2.0 * X / (f32) Image.Width;
                f32 OffY = (FilmY + HalfPixH) + HalfPixH * RandomBilateral(World.State);
                f32 OffX = (FilmX + HalfPixW) + HalfPixW * RandomBilateral(World.State);
                v3f RayDirection = Normalize(-CameraZ * FilmDist + CameraY * OffY * HalfFilmH + CameraX * OffX * HalfFilmW);
                            
                *ColorArrayOut += Raycast(World, Image, RayOrigin, RayDirection);   // M_k = M_{k-1} + (x_k - M_{k-1})/k

                *PixelOut++ = ARGBPack(*ColorArrayOut++/TotalSamples);
            }
        }

        // printf("Total Samples: %d\r", TotalSamples);
        // printf("r: %f, g: %f, b: %f \r ",ColorArray->r, ColorArray->g, ColorArray->b);
        // printf("r: %f, g: %f, b: %f \r ",ColorArray->r/TotalSamples, ColorArray->g/TotalSamples, ColorArray->b/TotalSamples);



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