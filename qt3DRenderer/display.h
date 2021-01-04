#pragma once
#include "vec3d.h"
#include "vec2d.h"
#include "tex2.h"

#include <cstdint>

extern bool USE_PAINTERS_ALGO;

class Display
{
public:
    Display();
    ~Display();

    // setup: allocates new color buffer
    void setup();

    // setSize: set a new screen size (i.e. color buffer)
    void setSize(const int& width, const int& height);

    // width: return the width of the 3D screen
    int width();

    // height: return the height of the 3D screen
    int height();

    // clearColorBuffer: fill color buffer with specific color
    void clearColorBuffer(const uint32_t& color);

    //
    void clearDepthBuffer(const float& depth);

    //
    uint32_t* colorBuffer();

    //
    float* depthBuffer();

    //
    void drawGrid();

    //
    void drawPixel(const int& x, const int& y, const uint32_t& color);

    //
    void drawPixel(const int& x, const int& y, const Vec4d& a, const Vec4d& b, const Vec4d& c, const uint32_t& color);

    //
    void drawTexel(const int& x, const int& y,
                   const Vec4d& a, const Vec4d& b, const Vec4d& c,
                   const Tex2& a_uv, const Tex2& b_uv, const Tex2& c_uv,
                   const uint32_t* texture, const int& textureWidth, const int& textureHeight,
                   const bool& fixDistortion = true);

    //
    void drawLine(const int& x1, const int& y1, const int& x2, const int& y2, const uint32_t& color);

    //
    void drawRect(const int& x, const int& y, const int& w, const int& h, const uint32_t& color);

    //
    void drawTriangle(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, const uint32_t& color);        

    //
    void fillTriangle(Vec4d p1, Vec4d p2, Vec4d p3, const uint32_t& color);

    //
    void drawTexturedTriangle(Vec4d p1, Vec4d p2, Vec4d p3,
                              Tex2 uv1, Tex2 uv2, Tex2 uv3,
                              const uint32_t* texture, const int& textureWidth, const int& textureHeight,
                              const bool& fixDistortion = true);

    // orthographic projection: objects appear to have the same size regardless of their Z distance
    // receives a 3D vector and returns a projected 2D point
    Vec2d project(Vec3d p);

private:
    void _fillFlatBottomTriangle(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, const uint32_t& color);
    void _fillFlatTopTriangle(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, const uint32_t& color);

    Vec3d _barycentricWeights(const Vec2d& a, const Vec2d& b, const Vec2d& c, const Vec2d& p);

    uint32_t* _colorBuffer;
    float* _depthBuffer;

    int _screenWidth;
    int _screenHeight;
    uint32_t _bkgColor;
    float _fovFactor;
};
