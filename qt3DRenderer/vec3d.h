#pragma once
#include "vec4d.h"

#include <iostream>

class Vec4d;

class Vec3d
{
public:
    Vec3d();
    Vec3d(const float& x, const float& y, const float& z);

    // data type conversion
    static Vec4d toVec4d(const Vec3d& v);

    Vec3d rotateZ(const float& angle);
    Vec3d rotateY(const float& angle);
    Vec3d rotateX(const float& angle);

    // magnitude: length of the hypothenuse
    float mag();

    // addition
    Vec3d add(const Vec3d& v);

    // subtraction
    Vec3d sub(const Vec3d& v);   

    // scalar multiplication
    Vec3d mul(const float& factor);        

    // division
    Vec3d div(const float& factor);

    // vector cross product
    Vec3d cross(const Vec3d& v);

    // scalar dot product
    float dot(const Vec3d& v);

    // normalize
    void norm();

    // overload addition operator to execute add()
    Vec3d operator+(const Vec3d& v)
    {
        return add(v);
    }

    // overload subtraction operator to execute sub()
    Vec3d operator-(const Vec3d& v)
    {
        return sub(v);
    }

    // overload multiplication operator to execute mul()
    Vec3d operator*(const float& factor)
    {
        return mul(factor);
    }

    // overload insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Vec3d& v)
    {
        os << "Vec3d=(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    float x, y;
    float z;
};
