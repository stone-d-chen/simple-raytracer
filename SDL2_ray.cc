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