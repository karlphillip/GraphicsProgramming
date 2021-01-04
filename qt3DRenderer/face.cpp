#include "face.h"

Face::Face(const int& a, const int& b, const int& c, const uint32_t& color)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->color = color;    
}

Face::Face(const int& a, const int& b, const int& c, const Tex2& uv1, const Tex2& uv2, const Tex2& uv3, const uint32_t& color)
{
    this->a = a;
    this->b = b;
    this->c = c;

    this->a_uv = uv1;
    this->b_uv = uv2;
    this->c_uv = uv3;

    this->color = color;
}
