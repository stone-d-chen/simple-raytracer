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

    world World = {};
    material Materials[] = 
    {
        {{0.3, 0.4, 0.5}, {0.0, 0.0, 0.0}, },
        {{0.0, 0.0, 0.0}, {0.8, 0.6, 0.7}, },
        {{0.0, 0.0, 0.0}, {0.0, 0.9, 0.3}, },  
    };
    sphere Spheres[] = 
    {
        {{0,0,0}, 1, 1},
    };
    plane Planes[] = 
    {
        {{0,0,1}, 1, 2},
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
    v3f Up = v3f{0, 0, 1};
    v3f CameraX = Normalize(Cross(Up, CameraZ)); //right hand rule
    v3f CameraY = Normalize(Cross(CameraZ, CameraX)); //right hand rule

    f32 FilmDist = 1.0;





    while(is_running == true)
    {

        u32 *Out = Image.Pixels;
        for(u32 Y = 0; Y < Image.Height; ++Y)
        {
            f32 FilmY = -1.0 + 2.0 * Y / (f32) Image.Height;
            for(u32 X = 0; X < Image.Width; ++X)
            {
                *Out++ = ARGBPack(255, 255, 0, 0);
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