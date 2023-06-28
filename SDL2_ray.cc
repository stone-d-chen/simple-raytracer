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

        u32 TileWidth = Image.Width / 8;
        u32 TileHeight = TileWidth;
        u32 TileCountX = (Image.Width + TileWidth - 1) / TileWidth;
        u32 TileCountY = (Image.Height + TileHeight - 1) / TileHeight;

        for(u32 TileY = 0; TileY < TileCountY; ++TileY)
        {
            u32 TileMinY = TileY * TileHeight;
            u32 TileOnePastMaxY = TileMinY + TileHeight; if(TileOnePastMaxY > Image.Height) TileOnePastMaxY = Image.Height;
            for(u32 TileX = 0; TileX < TileCountX; ++TileX)
            {
                u32 TileMinX = TileX * TileWidth;
                u32 TileOnePastMaxX = TileMinX + TileWidth; if(TileOnePastMaxX > Image.Width) TileOnePastMaxX = Image.Width;
                RenderTile(World, Image, TileMinX, TileMinY, TileOnePastMaxX, TileOnePastMaxY);
            }
        }

      


        void *TextureMemory = malloc(Image.Width * Image.Height * sizeof(u32));
        s32 Pitch = Image.Width * sizeof(u32);

        SDL_LockTexture(texture, NULL, &TextureMemory, &Pitch);
        memcpy(TextureMemory, (void*)Image.Pixels, Image.Width*Image.Height*sizeof(u32));

        SDL_UnlockTexture(texture);
        SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }
    return(0);

} 