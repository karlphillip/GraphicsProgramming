#include "vec4d.h"


Vec4d::Vec4d()
{
    x = y = z = 0;
    w = 1;
}

Vec4d::Vec4d(const float& x, const float& y, const float& z, const float& w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Vec3d Vec4d::toVec3d(const Vec4d& v)
{
    return Vec3d(v.x, v.y, v.z);
}

Vec2d Vec4d::toVec2d(const Vec4d& v)
{
    return Vec2d(v.x, v.y);
}

Vec4d Vec4d::mul(const Mat4& mat)
{
    Vec4d result;
    result.x = mat.m[0][0] * this->x + mat.m[0][1] * this->y + mat.m[0][2] * this->z + mat.m[0][3] * this->w;
    result.y = mat.m[1][0] * this->x + mat.m[1][1] * this->y + mat.m[1][2] * this->z + mat.m[1][3] * this->w;
    result.z = mat.m[2][0] * this->x + mat.m[2][1] * this->y + mat.m[2][2] * this->z + mat.m[2][3] * this->w;
    result.w = mat.m[3][0] * this->x + mat.m[3][1] * this->y + mat.m[3][2] * this->z + mat.m[3][3] * this->w;

    return result;
}

Vec3d Vec4d::normal(const Vec4d& a, const Vec4d& b, const Vec4d& c)
{
    // part of the Face Culling computation
    Vec3d vectorA = Vec4d::toVec3d(a);
    Vec3d vectorB = Vec4d::toVec3d(b);
    Vec3d vectorC = Vec4d::toVec3d(c);

    // 1. Find vectors B-A and C-A
    Vec3d vectorAB = vectorB - vectorA;
    Vec3d vectorAC = vectorC - vectorA;

    // normalize: adjust lengths to unit vector since we only care about their direction
    vectorAB.norm();
    vectorAC.norm();

    // 2. Take their cross product and find the perpendicular normal
    Vec3d faceNormal = vectorAB.cross(vectorAC); // this order for Left-Handed Coordinate System
    faceNormal.norm(); // normalize: adjust lengths to unit vector since we only care about their direction

    return faceNormal;
}
