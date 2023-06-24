#include <stdio.h>
#include "include/SDL.h"
#include "math.h"

#include "ray.cc"

typedef struct
{

} image_u32; 


int main(int ArgC, char **Args)
{
    bool is_running = false;
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {is_running = true; printf("Render\n");}

    u32 Width = 1280;
    u32 Height = 720;
    u32 *Pixels = (u32 *)malloc(Width * Height * sizeof(u32));

    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_POPUP_MENU);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);

    while(is_running == true)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        u32 *Out = Pixels;
        for(u32 Y = 0; Y < Height; ++Y)
        {
            for(u32 X = 0; X < Width; ++X)
            {
                *Out++ = ARGBPack(255, 255, 0, 0);
            }
        }

        SDL_UpdateTexture(texture, NULL, Pixels, (s32) Width * sizeof(u32));
        // TODO: Look into SDL_LockTexture

        SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(renderer);


    }
    return(0);

}