#pragma once
#include "vec3d.h"
#include "face.h"

#include <vector>


class Mesh
{
public:
    Mesh();
    Mesh(const uint32_t* texData, const int& texWidth, const int& texHeight);
    void setTexture(const uint32_t* texData, const int& texWidth, const int& texHeight);

    std::vector<Vec3d> vertices;
    std::vector<Face> faces;

    std::shared_ptr<uint32_t[]> texture;
    int textureWidth;
    int textureHeight;

    Vec3d rotation;                     // current rotation of the mesh
    Vec3d scale;                        // current scale
    Vec3d translation;                  // current translation
};
