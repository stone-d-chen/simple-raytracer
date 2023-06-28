
world World = {};
material Materials[] =
{
    /* 0 */ {{0.7, 0.8, 0.8}, {0.0, 0.0, 0.0} , 0 },
    /* 1 */ {{0, 0, 0}, {0.1, 0.5, 0.1}, 0.7},
    /* 2 */ {{0.0, 0.0, 0.0}, {0.8, 0.6, 0.4} , 0 },
    /* 3 */ {{0.0, 0.0, 0.0}, {0.3, 0.9, 0.3} , 1 },
};
plane Planes[] =
{
    {{0,0,1}, 0, 1}, 
};
sphere Spheres[] =
{
    {{0,0,0}, 1, 2},
};
World.MaterialCount = ArrayCount(Materials);
World.SphereCount = ArrayCount(Spheres);
World.PlaneCount = ArrayCount(Planes);
World.Materials = Materials;
World.Spheres = Spheres;
World.Planes = Planes;

random_series State = {(u32)rand()};
World.State = &State;