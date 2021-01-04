#include "mesh.h"

#include <cstring>


Mesh::Mesh()
{
    scale       = Vec3d(1.f, 1.f, 1.f);
    rotation    = Vec3d(0.f, 0.f, 0.f);
    translation = Vec3d(0.f, 0.f, 0.f);

    textureWidth = textureHeight = 0;
}

Mesh::Mesh(const uint32_t* texData, const int& texWidth, const int& texHeight)
{
    scale       = Vec3d(1.f, 1.f, 1.f);
    rotation    = Vec3d(0.f, 0.f, 0.f);
    translation = Vec3d(0.f, 0.f, 0.f);

    setTexture(texData, texWidth, texHeight);
}

void Mesh::setTexture(const uint32_t* texData, const int& texWidth, const int& texHeight)
{
    // deep copy texture data
    int texSize = texWidth * texHeight;
    texture = std::shared_ptr<uint32_t[]>(new uint32_t[texSize]);
    std::memcpy(texture.get(), (uint32_t*)texData, texSize * sizeof(uint32_t));

    // initialize texture dimensions
    textureWidth = texWidth;
    textureHeight = texHeight;
}
