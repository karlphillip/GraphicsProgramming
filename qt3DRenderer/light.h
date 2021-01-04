#pragma once
#include "vec3d.h"

class Light
{
public:
    Light();

    // calculate new color value based on a percentage factor that represents the light intensity
    static uint32_t calcIntensity(const uint32_t& color, float intensityPercent);

    Vec3d direction;
};
