# Simple Raytracer

## 

## Updates/Notes

2023/29/6

-   Start separating out platform layer and raycasting layer

2023/28/6

-   Need to seed a random number for each work_order? right now going through World.State pointer

Okay so here's the current issue, right now my random_state \*State is baked into my world struct and my RenderTile call goes RenderTile(World, Image) ....

Actually maybe less of a problem than I thought, since I pack more work orders with a copy of the `world World` struct, so once the world is packed, then I just update the struct with a new seed

``` cpp
typedef struct {
    u32 MaterialCount;
    material *Materials;
    //other stuff
    random_series *State;
} world;

void RenderTile(world World, image_u32 Image, ...)
  
//work order packing pseudo code
for(u32 TileY = 0; TileY < TileCountY; ++TileY)
  for(u32 TileX = 0; TileX < TileCountX; ++TileX)
    work_order *Order = Queue.Orders + Queue.WorkOrderCount++;
    // ...
    Order->World = World; // we get a copy to the world
    Order->Image = Image;
    // ...
    
    //something like
    Order->World.State =  rand();
```

-   bug fixed ARGB back (shifted way too much), add specularity
-   rays per pixel, implement floating point safe accumulation
-   Everything is too slow, should just multithread

## Multi-threading notes

<https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread>

``` cpp
HANDLE CreateThread(
  lpThreadAttributes, // NULL
  dwStackSize,    // 0
  lpStartAddress, // ptr to function to be executed by thread
  lpParameter, // a pointer to the variable to be passed to the thread
  dwCreationFlags, // 0 immediately run
  lpThreadId // NULL;
);

DWORD WINAPI ThreadProc( // pass a pointer to this function
  LPVOID lpParameter //thread receives pointer to the data
);
```

-   So I want to convert RenderTile into a thread routine
-   I want the thread routine to continuously compute tiles until there's nothing left
-   So it should take a work queue with the list of tiles to compute

2023/25/6

-   added bounces + random

Made a horrible mistake, I set the seed in the World struct like below and called the functions like

``` cpp
struct {
  u32 state;
} random_series;

struct {
  random_series State;
} world;

for(u32 Row = 0; ...)
  for(u32 Col = 0; ...)
    Raycast(World, Image, ...)
  
void Raycast(world World, image_u32 Image, ...) {
    RandomBilateral(&World.State);
}
```

So every bounce of the Raycast got a new state, but every new call to Raycast got the original world state which was borking the whole thing.

So two options

-   I can have the World be passed by reference or
-   update the world struct to contain a pointer to the random series which will live "globally"

Probably the second since it already contains pointers to the spheres and planes

``` cpp
world World = {};

void Raycast(world *World, ...)
  
// or

struct {
  random_series *State;
} world;

random_series State = {};
State.state = rand();
World.State = &State
// (I should probably come up with better terminology…)
```

2023/24/6

-   Trying to separate out the SDL2 layer from everything else, ray, SDL_ray, math

-   Init SDL2, might want to change UpdateTexture to [SDL_LockTexture()](https://wiki.libsdl.org/SDL2/SDL_LockTexture) apparently
