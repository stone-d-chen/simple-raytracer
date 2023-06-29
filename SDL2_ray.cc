#include <stdio.h>
#include "include/SDL.h"

#include "math.h"
#include "ray.cc"

// my platform is now SDL2/Windows
#include <windows.h>
u32 AddAndReturnPreviousValue(volatile u32 *Addend, u32 Value)
{
    u32 Result = InterlockedExchangeAdd(Addend, Value);
    return(Result);
}
DWORD WINAPI WorkerThread(void *Param)
{
    work_queue *Queue = (work_queue*) Param;
    RenderTile(Queue);
    return(0);
}
void CreateWorkerThread(void *Param)
{
    HANDLE Handle = CreateThread(NULL, 0, WorkerThread, Param, 0, NULL);
    CloseHandle(Handle);
}

image_u32 AllocateImage(u32 Width, u32 Height)
{
    image_u32 Image = {};
    Image.Width = Width;
    Image.Height = Height;
    Image.Pixels = (u32 *)malloc(Width * Height * sizeof(u32));
    return(Image);
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

#include "WorldInit.h" //just to get it out of the way

    u32 TileWidth = Image.Width / 8;
    u32 TileHeight = TileWidth;
    u32 TileCountX = (Image.Width + TileWidth - 1) / TileWidth;
    u32 TileCountY = (Image.Height + TileHeight - 1) / TileHeight;
    u32 TotalTileCount = TileCountX * TileCountY;

    work_queue Queue = {};    
    Queue.Orders = (work_order *)malloc(TotalTileCount * sizeof(work_order));

    while(is_running == true)
    {

        u32 FrameStart = SDL_GetTicks();

        Queue.NextWorkOrder = 0;
        Queue.TilesRetired = 0;
        Queue.WorkOrderCount = 0;
       
        for(u32 TileY = 0; TileY < TileCountY; ++TileY)
        {
            u32 TileMinY = TileY * TileHeight;
            u32 TileOnePastMaxY = TileMinY + TileHeight; if(TileOnePastMaxY > Image.Height) TileOnePastMaxY = Image.Height;
            for(u32 TileX = 0; TileX < TileCountX; ++TileX)
            {
                u32 TileMinX = TileX * TileWidth;
                u32 TileOnePastMaxX = TileMinX + TileWidth; if(TileOnePastMaxX > Image.Width) TileOnePastMaxX = Image.Width;

                work_order *Order = Queue.Orders + Queue.WorkOrderCount++;
                Order->World = World;
                Order->Image = Image;
                Order->TileMinX = TileMinX;
                Order->TileMinY = TileMinY;
                Order->TileOnePastMaxX = TileOnePastMaxX;
                Order->TileOnePastMaxY = TileOnePastMaxY;
                Order->World.State = CreateRandomSeries();
            }
        }

        AddAndReturnPreviousValue(&Queue.TilesRetired, 0);

        u32 CoreCount = 14;
        for(u32 CoreIdx = 1; CoreIdx < CoreCount; ++CoreIdx)
        {
            CreateWorkerThread((void*)&Queue);
        }
        RenderTile(&Queue);
      
        void *TextureMemory = malloc(Image.Width * Image.Height * sizeof(u32));
        s32 Pitch = Image.Width * sizeof(u32);
        SDL_LockTexture(texture, NULL, &TextureMemory, &Pitch);
        memcpy(TextureMemory, (void*)Image.Pixels, Image.Width*Image.Height*sizeof(u32));
        SDL_UnlockTexture(texture);

        SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        u32 FrameEnd = SDL_GetTicks();
        u32 FrameTime = FrameEnd - FrameStart;

        printf("MS Elapsed: %d MS   FPS: %.2f \r", FrameTime, 1000.0 / (f32) FrameTime );


        // user_inputs UserInput = {}
        // FillUserInput(UserInput);
        // void RaycastUpdate(World, Image, UserInput)
        // {
        //     UserInput.
        // }

        SDL_Event event;
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
            {
                is_running = false;
            } break;

            case SDL_KEYDOWN:
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        is_running = false;
                    } break;
                    case SDLK_UP:
                    {
                        World.Camera.P.z += 0.02;
                    } break;
                    case SDLK_DOWN:
                    {
                        World.Camera.P.z -= 0.02;
                    } break;
                    case SDLK_RIGHT:
                    {
                        World.Camera.P.x += 0.02;
                    } break;
                    case SDLK_LEFT:
                    {
                        World.Camera.P.x -= 0.02;
                    } break;
                    case SDLK_w:
                    {
                        World.Camera.P.y += 0.06;
                        World.Camera.LookAt.y += 0.06;
                    } break;
                    case SDLK_s:
                    {
                        World.Camera.LookAt.y -= 0.06;
                        World.Camera.P.y -= 0.06;
                    } break;
                    case SDLK_d:
                    {
                        World.Camera.P.x += 0.02;
                        World.Camera.LookAt.x += 0.02;
                        
                    } break;
                    case SDLK_a:
                    {
                        World.Camera.P.x -= 0.02;
                        World.Camera.LookAt.x -= 0.02;
                    } break;
                }
            } break;
        }
    }
    return(0);

} 