#pragma once
#include "mesh.h"

#include <string>


class OBJLoader
{
public:
    OBJLoader(const std::string& filename);
    Mesh mesh();

private:
    Mesh _mesh;
};
