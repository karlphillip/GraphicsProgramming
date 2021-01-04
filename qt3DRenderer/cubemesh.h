#pragma once
#include "mesh.h"


class CubeMesh : public Mesh
{
public:
    CubeMesh();
    CubeMesh(const uint32_t* texData, const int& texWidth, const int& texHeight);

private:
    void _init();
};
