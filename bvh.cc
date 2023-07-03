

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
    Node.bbMax = v3f{FLT_MIN, FLT_MIN, FLT_MIN};

    for(u32 First = Node.FirstPrim, i = 0; i < Node.PrimCount; ++i)
    {
        sphere &Sphere = Spheres[First + i];
        Node.bbMin = fminf(Node.bbMin, Sphere.P - Sphere.r);
        Node.bbMax = fmaxf(Node.bbMax, Sphere.P - Sphere.r);
    }
}
void SubdivideNode(u32 NodeIdx, sphere *Spheres)
{
    bvh_node &Node = SphereBVH[NodeIdx];
    if(Node.PrimCount <= 2) return;

    v3f Extent = Node.bbMax - Node.bbMin;
    u32 axis = 0;
    if(Extent.y - Extent.x) axis = 1;
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