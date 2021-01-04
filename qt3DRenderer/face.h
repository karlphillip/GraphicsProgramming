#pragma once
#include <stdint.h>

#include "tex2.h"

class Face
{
public:
    Face(const int& a, const int& b, const int& c, const uint32_t& color = 0xFF000000);
    Face(const int& a, const int& b, const int& c, const Tex2& uv1, const Tex2& uv2, const Tex2& uv3, const uint32_t& color = 0xFF000000);

    int a, b, c;            // the indexes of a vertex array that define a single Face (triangle)

    Tex2 a_uv, b_uv, c_uv;     // the texture coordinates associated with each vertex

    uint32_t color;
};
