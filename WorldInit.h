
world World = {};
material Materials[] =
{
    {{0.3, 0.4, 0.5}, {0.0, 0.0, 0.8} },
    {{0.0, 0.0, 0.0}, {0.8, 0.6, 0.7} },
    {{0.0, 0.0, 0.0}, {0.0, 0.9, 0.3} },
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

random_series State = {rand()};
World.State = &State;







f32 FilmDist = 1.0;
f32 FilmW = 1.0;
f32 FilmH = 1.0;
if (Image.Width > Image.Height)
{
    FilmH *= (f32)Image.Height / Image.Width;
}
if (Image.Width < Image.Height)
{
    FilmW *= (f32)Image.Width / Image.Height;
}
f32 HalfFilmW = 0.5 * FilmW;
f32 HalfFilmH = 0.5 * FilmH;


v3f CameraP = v3f{ 0, -10, 1 };
v3f LookAt = v3f{ 0, 0, 0, };
v3f CameraZ = Normalize(CameraP - LookAt);
v3f CameraX = Normalize(Cross(v3f{ 0,0,1 }, CameraZ)); //right hand rule
v3f CameraY = Normalize(Cross(CameraZ, CameraX)); //right hand rule

