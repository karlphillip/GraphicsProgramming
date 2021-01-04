#pragma once
#include "vec2d.h"
#include "vec3d.h"
#include "mat4.h"

#include <iostream>

class Vec2d;
class Vec3d;


class Vec4d
{
public:
    Vec4d();    
    Vec4d(const float& x, const float& y, const float& z, const float& w);

    // data type conversions
    static Vec3d toVec3d(const Vec4d& v);
    static Vec2d toVec2d(const Vec4d& v);

    // matrix multiplication of Vec4d with Mat4
    Vec4d mul(const Mat4& m);

    // overload multiplication operator to execute mul()
    Vec4d operator*(const Mat4& m)
    {
        return mul(m);
    }

    // normal: a helper function to calculate the Normal from 3 vectors
    static Vec3d normal(const Vec4d& a, const Vec4d& b, const Vec4d& c);

    // overload insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Vec4d& vec)
    {
        os << "Vec4d=(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return os;
    }

    float x, y;
    float z, w;
};

