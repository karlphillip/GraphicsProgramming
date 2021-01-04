#include "triangle.h"

Triangle::Triangle()
{

}

Triangle::Triangle(const Vec4d& p1, const Vec4d& p2, const Vec4d& p3)
{
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;

    this->color = 0xFFFFFFFF;
}

Triangle::Triangle(const Vec4d& p1, const Vec4d& p2, const Vec4d& p3,
                   const Tex2& t1, const Tex2& t2, const Tex2& t3,
                   const std::shared_ptr<uint32_t[]>& texData,
                   const int texWidth, const int texHeight,
                   const uint32_t& color)
{
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;

    texCoords[0] = t1;
    texCoords[1] = t2;
    texCoords[2] = t3;

    this->texture = texData;
    this->textureWidth = texWidth;
    this->textureHeight = texHeight;

    this->color = color;    
}
