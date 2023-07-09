
#include "ray.h"
world World = {};
material Materials[] = 
{ 
    {{0.5, 0.7, 1.0}, {                  }, 0.0, 0.0},//0
    {{0, 30/255.0, 0/255.0 }, {56/255.0, 229/255.0, 77/225.0}, 0.5, 0.0},//1
    {{             }, { 0.70, 0.5, 0.3   }, 0.3, 0.0},//2
    {{             }, { 0.9, 0.2, 0.2    }, 0.2, 0.0},//3
    {{             }, { 0.95, 0.95, 0.95 }, 1.0, 0.0},//4
    {{0.75, 0.75, 0.75}, {                  }, 0.0, 0.0},//5
    {{             }, { 0.95,0.6,0.95    }, 1.0, 0.0},//6
    {{             }, { 1.1,1.1,1.1     }, 0.0, 1.3},//7
    {{             }, { 33/255.0, 146/255.0, 255/255.0 }, 0.5, 0.0},//8
    {{1.5, 1.5, 1.5}, {                  }, 0.0, 0.0},//9
    {{             }, { 1.1,0.8,1.1     }, 0.0, 1.3},//10
    {{1.1,1.1,1.1},   {                 }, 0.0, 0.0},//11
    {{            },   {235/255.0, 174/255.0, 52/255.0 }, 0.0, 0.0},//12
    {{            },   {1, 1, 1 }, 1.0, 0.0},//13

};
plane Planes[] =
{
    {{ 0,0,1 }, 1, 1},
    // {{ 0,0,1 }, -10.0, 2},
};
sphere Spheres[] = 
{
    {{ 0, 0, 0 }, 1.0, 7},
    {{ 0, 3, 1 }, 1.0, 3},
    {{ 0, 3, -0.5 }, 0.35, 8},
    {{ -3.5, 0, 1.5}, 1.0, 11},
    {{ 0, 0, 6.0}, 3.0, 11},
    {{ 2.0, 1, -0.75}, 1.0, 9},
    {{ 3.5, 0, 0.5}, 1.0, 10},
    {{ 1.5, 1.0, 0.75}, 0.5, 8},
    {{ -1.5, -1.0, 1.5}, 0.35, 3},
    {{ -7.0, 1.0, 0.75}, 0.5, 8},
    {{ -0.5, -7.0, 0.5}, 0.2, 7},
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

World.RaysPerPixel = 1;


// user_inputs Inputs = {};
