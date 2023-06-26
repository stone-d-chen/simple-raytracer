# Simple Raytracer

## Updates/Notes

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
