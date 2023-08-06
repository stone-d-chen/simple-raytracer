
#include "math.h"
#include "bvh.h"
#include "ray.h"

void SwapSpheres(sphere *A, sphere *B)
{
    sphere Temp = *A;
    *A = *B;
    *B = Temp;
}

void UpdateSphereNodeBounds(u32 NodeIdx, sphere *Spheres)
{
    bvh_node &Node = SphereBVH[NodeIdx];
    Node.bbMin = v3f{FLT_MAX, FLT_MAX, FLT_MAX};
    Node.bbMax = v3f{-FLT_MAX, -FLT_MAX, -FLT_MAX};

    for(u32 First = Node.FirstPrim, i = 0; i < Node.PrimCount; ++i)
    {
        sphere &Sphere = Spheres[First + i];
        Node.bbMin = fminf(Node.bbMin, Sphere.P - Sphere.r);
        Node.bbMax = fmaxf(Node.bbMax, Sphere.P + Sphere.r);
    }
}
void SubdivideNode(u32 NodeIdx, sphere *Spheres)
{
    bvh_node &Node = SphereBVH[NodeIdx];
    if(Node.PrimCount <= 2) return;

    v3f Extent = Node.bbMax - Node.bbMin;
    u32 axis = 0;
    if(Extent.y > Extent.x) axis = 1;
    if(Extent.z > Extent.v[axis]) axis = 2;
    f32 SplitPosition = Node.bbMin.v[axis] + Extent.v[axis] * 0.5f;

    u32 i = Node.FirstPrim;
    u32 j = i + Node.PrimCount - 1;

    while (i <= j)
    {
        if (Spheres[i].P.v[axis] < SplitPosition)
        {
            i++;
        }
        else
        {
            SwapSpheres(&Spheres[i], &Spheres[j--]);
        }
    }

    u32 LeftCount = i - Node.FirstPrim;
    if(LeftCount == 0 || LeftCount == Node.PrimCount) return;

    u32 LeftChildIdx = NodesUsed++;
    u32 RightChildIdx = NodesUsed++;
    Node.LeftChild = LeftChildIdx;
    Node.RightChild = RightChildIdx;
    SphereBVH[LeftChildIdx].FirstPrim = Node.FirstPrim;
    SphereBVH[LeftChildIdx].PrimCount = LeftCount;
    SphereBVH[RightChildIdx].FirstPrim = i;
    SphereBVH[RightChildIdx].PrimCount = Node.PrimCount - LeftCount;
    Node.PrimCount = 0;

    UpdateSphereNodeBounds(LeftChildIdx , Spheres);
    UpdateSphereNodeBounds(RightChildIdx, Spheres);
    SubdivideNode(LeftChildIdx, Spheres);
    SubdivideNode(RightChildIdx, Spheres);
}

bool IntersectAABB(v3f RayDirection, v3f RayOrigin, v3f AABBMin, v3f AABBMax, f32 &HitDistance)
{
    f32 tx1 = (AABBMin.x - RayOrigin.x) / RayDirection.x;
    f32 tx2 = (AABBMax.x - RayOrigin.x) / RayDirection.x;
    f32 tmin = fminf(tx1, tx2);
    f32 tmax = fmaxf(tx1, tx2);
    f32 ty1 = (AABBMin.y - RayOrigin.y) / RayDirection.y;
    f32 ty2 = (AABBMax.y - RayOrigin.y) / RayDirection.y;
    tmin = fmaxf(tmin, fminf(ty1, ty2));
    tmax = fminf(tmax, fmaxf(ty1, ty2));
    f32 tz1 = (AABBMin.z - RayOrigin.z) / RayDirection.z;
    f32 tz2 = (AABBMax.z - RayOrigin.z) / RayDirection.z;
    tmin = fmaxf(tmin, fminf(tz1, tz2));
    tmax = fminf(tmax, fmaxf(tz1, tz2));
    return(tmax >= tmin && tmin < HitDistance);
}

void IntersectBVH(v3f RayDirection, v3f RayOrigin, const u32 NodeIndex, sphere *Spheres,
                  f32 &HitDistance, u32 &HitMatIndex, v3f &NextOrigin, v3f &NextNormal)
{
    bvh_node Node = SphereBVH[NodeIndex];
    if(!IntersectAABB(RayDirection, RayOrigin, Node.bbMin, Node.bbMax, HitDistance)) return;
    if(Node.PrimCount != 0)
    {
        f32 Tolerance = 0.01f;
        for(u32 SphereIndex = 0; SphereIndex <  Node.PrimCount; ++SphereIndex)
        {
            sphere Sphere = Spheres[Node.FirstPrim + SphereIndex];
            f32 a = Inner(RayDirection, RayDirection);
            v3f OffsetSphereOrigin = RayOrigin - Sphere.P;
            f32 b = 2.0 * Inner(RayDirection, OffsetSphereOrigin);
            f32 c = Inner(OffsetSphereOrigin, OffsetSphereOrigin) - Sphere.r * Sphere.r;
            f32 RootTerm = sqrtf(b*b - 4.0f*a*c);
            if(RootTerm > Tolerance)
            {
                f32 t = (-b + RootTerm) / (2.0 * a);
                f32 tn = (-b - RootTerm) / (2.0 * a);
                if(tn > Tolerance)
                {
                    t = tn;
                }
                if(t < HitDistance && t > Tolerance)
                {
                    HitDistance = t;
                    HitMatIndex = Sphere.MatIndex;
                    NextOrigin = t*RayDirection + RayOrigin;
                    NextNormal = Normalize(NextOrigin - Sphere.P);
                }
            }
        }
    }
    else
    {
        IntersectBVH(RayDirection, RayOrigin, Node.LeftChild, Spheres , HitDistance, HitMatIndex, NextOrigin, NextNormal);
        IntersectBVH(RayDirection, RayOrigin, Node.RightChild, Spheres, HitDistance, HitMatIndex, NextOrigin, NextNormal);
    }
}