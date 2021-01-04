#pragma once
#include "vec3d.h"
#include "vec4d.h"
#include "triangle.h"

#include <vector>


enum FRUSTUM_PLANE
{
    LEFT   = 0,
    RIGHT  = 1,
    TOP    = 2,
    BOTTOM = 3,
    NEAR   = 4,
    FAR    = 5
};


class Plane
{
public:
    Plane();

    Vec3d point;
    Vec3d normal;
};


class Polygon
{
public:
    // Constructor: receive the vertices for a triangle and its associated texture coordinates
    Polygon(const Vec3d& v1, const Vec3d& v2, const Vec3d& v3, const Tex2& t1, const Tex2& t2, const Tex2& t3);

    // clip: performs polygon clipping based on each of the Frustum sides
    void clip(const Plane frustumPlanes[6]);

    // triangles: breaks down the vertices vector into one or more Triangle objects
    std::vector<Triangle> triangles();

    std::vector<Vec3d> vertices;
    std::vector<Tex2> texCoords;

private:
    void _clipAgainstPlane(const Plane& frustumPlane);
};
