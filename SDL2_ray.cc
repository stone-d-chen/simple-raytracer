#include <stdio.h>

#include "include/imgui.h"
#include "include/imgui_impl_sdl2.h"
#include "include/imgui_impl_sdlrenderer2.h"
#include "include/SDL.h"



#include "math.h"
#include "bvh.cc"
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
    Image.V3FColorArray = (v3f *)malloc(sizeof(v3f) * Image.Height * Image.Width);
    Image.Contributions = 1;
    return(Image);
}

int main(int ArgC, char **Args)
{
    bool is_running = false;
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {is_running = true; printf("Render\n");}

    image_u32 Image = AllocateImage(1280, 720);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Image.Width, Image.Height, window_flags);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Image.Width, Image.Height);
    void *TextureMemory = malloc(Image.Width * Image.Height * sizeof(u32));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);





#include "WorldInit.h" //just to get it out of the way

    bvh_node &Root = SphereBVH[RootNodeIdx];
    Root.PrimCount = ArrayCount(Spheres);
    Root.FirstPrim = 0;
    Root.LeftChild = 0;
    Root.RightChild = 0;
    UpdateSphereNodeBounds(RootNodeIdx, Spheres);
    SubdivideNode(RootNodeIdx, Spheres);

    u32 TileWidth = Image.Width / 14;
    u32 TileHeight = TileWidth;
    u32 TileCountX = (Image.Width + TileWidth - 1) / TileWidth;
    u32 TileCountY = (Image.Height + TileHeight - 1) / TileHeight;
    u32 TotalTileCount = TileCountX * TileCountY;

    work_queue Queue = {};    
    Queue.Orders = (work_order *)malloc(TotalTileCount * sizeof(work_order));

    while(is_running == true)
    {

        // u32 FrameStart = SDL_GetTicks();

        // user_inputs UserInputs = {};

        // SDL_Event event;
        // SDL_PollEvent(&event);
        // switch(event.type)
        // {
        //     case SDL_QUIT:
        //     {
        //         is_running = false;
        //     } break;

        //     case SDL_KEYDOWN:
        //     {
        //         switch(event.key.keysym.sym)
        //         {
        //             case SDLK_ESCAPE:
        //             {
        //                 is_running = false;
        //             } break;
        //             case SDLK_UP:
        //             {
        //                 UserInputs.Up = 1;
        //             } break;
        //             case SDLK_DOWN:
        //             {
        //                 UserInputs.Down = 1;
        //             } break;
        //             case SDLK_RIGHT:
        //             {
        //                 UserInputs.Right = 1;
        //             } break;
        //             case SDLK_LEFT:
        //             {
        //                 UserInputs.Left = 1;
        //             } break;
        //             case SDLK_w:
        //             {
        //                 UserInputs.W = 1;
        //             } break;
        //             case SDLK_s:
        //             {
        //                 UserInputs.S = 1;
        //             } break;
        //             case SDLK_d:
        //             {
        //                 UserInputs.D = 1;                        
        //             } break;
        //             case SDLK_a:
        //             {
        //                 UserInputs.A = 1;                        
        //             } break;
        //         }
        //     } break;
        // }

        // UpdateWorldState(&World, &Image, UserInputs);

        // Queue.NextWorkOrder = 0;
        // Queue.TilesRetired = 0;
        // Queue.WorkOrderCount = 0;
       
        // for(u32 TileY = 0; TileY < TileCountY; ++TileY)
        // {
        //     u32 TileMinY = TileY * TileHeight;
        //     u32 TileOnePastMaxY = TileMinY + TileHeight; if(TileOnePastMaxY > Image.Height) TileOnePastMaxY = Image.Height;
        //     for(u32 TileX = 0; TileX < TileCountX; ++TileX)
        //     {
        //         u32 TileMinX = TileX * TileWidth;
        //         u32 TileOnePastMaxX = TileMinX + TileWidth; if(TileOnePastMaxX > Image.Width) TileOnePastMaxX = Image.Width;

        //         work_order *Order = Queue.Orders + Queue.WorkOrderCount++;
        //         Order->World = World;
        //         Order->Image = Image;
        //         Order->TileMinX = TileMinX;
        //         Order->TileMinY = TileMinY;
        //         Order->TileOnePastMaxX = TileOnePastMaxX;
        //         Order->TileOnePastMaxY = TileOnePastMaxY;
        //         Order->World.State = CreateRandomSeries();
        //     }
        // }

        // AddAndReturnPreviousValue(&Queue.TilesRetired, 0);

        // u32 CoreCount = 2;
        // for(u32 CoreIdx = 1; CoreIdx < CoreCount; ++CoreIdx)
        // {
        //     CreateWorkerThread((void*)&Queue);
        // }

        // while(Queue.TilesRetired < Queue.WorkOrderCount)
        // {
            
        // }
        // RenderTile(&Queue);
        // Image.Contributions++;
      
        // s32 Pitch = Image.Width * sizeof(u32);
                                
        // SDL_LockTexture(texture, NULL, &TextureMemory, &Pitch);// lock portion of texture
        // memcpy(TextureMemory, (void*)Image.Pixels, Image.Width*Image.Height*sizeof(u32));
        // SDL_UnlockTexture(texture);
        //                                 // blit portion of texture

        
        // SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
        // SDL_RenderPresent(renderer);
        // SDL_RenderClear(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                is_running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                is_running = false;
        }
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);





        // u32 FrameEnd = SDL_GetTicks();
        // u32 FrameTime = FrameEnd - FrameStart;

        // printf("MS Elapsed: %d MS   FPS: %.2f \r", FrameTime, 1000.0 / (f32) FrameTime );



    
    }
    return(0);

} 