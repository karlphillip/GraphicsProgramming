#pragma once
#include "vec2d.h"
#include "tex2.h"

#include <stdint.h>

#include <vector>


class Triangle
{
public:
    Triangle();

    Triangle(const Vec4d& p1, const Vec4d& p2, const Vec4d& p3);

    Triangle(const Vec4d& p1, const Vec4d& p2, const Vec4d& p3,
             const Tex2& t1, const Tex2& t2, const Tex2& t3,
             const std::shared_ptr<uint32_t[]>& texData = nullptr,
             const int texWidth = 0, const int texHeight = 0,
             const uint32_t& color = 0xFFFFFFFF);

    Vec4d points[3];
    Tex2 texCoords[3];

    std::shared_ptr<uint32_t[]> texture;
    int textureWidth;
    int textureHeight;

    uint32_t color;
};

