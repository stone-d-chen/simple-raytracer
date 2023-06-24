#include "ray.h"
#include "math.h"

u32 ARGBPack(v3f rgb)
{
    u32 r = (u32) (255 * rgb.r) << 24 |
            (u32) (255 * rgb.g) << 16 |
            (u32) (255 * rgb.b) << 0  |
            0xFF000000;
    return(r);
}

u32 ARGBPack(u32 A, u32 R, u32 G, u32 B)
{
    u32 r = (A << 24 | R << 16 | G << 8 | B << 0);
    return(r);
}

image_u32 AllocateImage(u32 Width, u32 Height)
{
    image_u32 Image = {};
    Image.Width = Width;
    Image.Height = Height;
    Image.Pixels = (u32 *)malloc(Width * Height * sizeof(u32));
    return(Image);
}