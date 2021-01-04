#include "vec2d.h"

#include <cmath>


Vec2d::Vec2d()
{
    x = y = 0;
}

Vec2d::Vec2d(const float& x1, const float& y1)
{
    x = x1;
    y = y1;
}

float Vec2d::mag() const
{
    // pythagoras theorem
    return std::sqrt(x*x + y*y);
}

Vec2d Vec2d::add(const Vec2d& v) const
{
    return Vec2d(this->x + v.x, this->y + v.y);
}

Vec2d Vec2d::sub(const Vec2d& v) const
{
    return Vec2d(this->x - v.x, this->y - v.y);
}

Vec2d Vec2d::mul(const float& factor) const
{
    return Vec2d(this->x * factor, this->y * factor);
}

Vec2d Vec2d::div(const float& factor) const
{
    return Vec2d(this->x / factor, this->y / factor);
}

float Vec2d::dot(const Vec2d& v) const
{
    return (this->x * v.x) + (this->y * v.y);
}

void Vec2d::norm()
{
    float magnitude = mag();
    this->x /= magnitude;
    this->y /= magnitude;
}
