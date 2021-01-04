#pragma once
#include "vec3d.h"

#include <iostream>

class Vec3d;


class Vec2d
{
public:
    Vec2d();
    Vec2d(const float& x1, const float& y1);

    // magnitude: length of the hypothenuse
    float mag() const;

    // addition
    Vec2d add(const Vec2d& v) const;

    // subtraction
    Vec2d sub(const Vec2d& v) const;

    // multiplication
    Vec2d mul(const float& factor) const;

    // division
    Vec2d div(const float& factor) const;

    // scalar dot product
    float dot(const Vec2d& v) const;

    // normalize
    void norm();

    // overload insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Vec2d& vec)
    {
        os << "Vec2d=(" << vec.x << ", " << vec.y << ")";
        return os;
    }

    float x;
    float y;
};
