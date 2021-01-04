#pragma once
#include <stdint.h>

class Tex2
{
public:
    Tex2(const float& u = 0, const float& v = 0);

    float u;
    float v;
};


extern int REDBRICK_WIDTH;
extern int REDBRICK_HEIGHT;
extern uint8_t REDBRICK_TEXTURE[];
