#include "light.h"

Light::Light()
{    
}

uint32_t Light::calcIntensity(const uint32_t& color, float intensityPercent)
{
    // clamp range between 0.0 and 1.0
    if (intensityPercent < 0.f) intensityPercent = 0.f;
    if (intensityPercent > 1.f) intensityPercent = 1.f;

    uint32_t a = (color & 0xFF000000);
    uint32_t r = (uint32_t)((color & 0x00FF0000) * intensityPercent);
    uint32_t g = (uint32_t)((color & 0x0000FF00) * intensityPercent);
    uint32_t b = (uint32_t)((color & 0x000000FF) * intensityPercent);

    uint32_t newColor = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return newColor;
}
