
#include "ray.h"
world World = {};
material Materials[] =
{
    /* 0 */ {{0.7, 0.8, 0.8}, {0.0, 0.0, 0.0} , 0 },
    /* 1 */ {{0, 0, 0}, {0.1, 0.5, 0.1}, 0.7},
    /* 2 */ {{0.0, 0.0, 0.0}, {0.8, 0.6, 0.4} , 0 },
    /* 3 */ {{0.0, 0.0, 0.0}, {0.6, 0.6, 0.6} , 1 },
};
plane Planes[] =
{
    {{0,0,1}, 0, 1}, 
};
sphere Spheres[] =
{
    {{0,0,0}, 1, 2},
    {{3,-2,1.5}, 1, 3},
    {{-3,-2,1.5}, 1, 2},
};
World.MaterialCount = ArrayCount(Materials);
World.SphereCount = ArrayCount(Spheres);
World.PlaneCount = ArrayCount(Planes);
World.Materials = Materials;
World.Spheres = Spheres;
World.Planes = Planes;

random_series State = CreateRandomSeries();
World.State = State;

World.Camera.P      = v3f{ 0, -10, 1 };
World.Camera.LookAt = v3f{ 0, 0, 0, };
World.Camera.Z      = Normalize(World.Camera.P - World.Camera.LookAt);
World.Camera.X      = Normalize(Cross(v3f{ 0,0,1 }, World.Camera.Z)); //right hand rule
World.Camera.Y      = Normalize(Cross(World.Camera.Z, World.Camera.X)); //right hand rule
