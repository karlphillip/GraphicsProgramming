#include "cubemesh.h"


CubeMesh::CubeMesh()
{
    _init();
}

CubeMesh::CubeMesh(const uint32_t* texData, const int& texWidth, const int& texHeight)
: Mesh(texData, texWidth, texHeight)
{
    _init();
}

void CubeMesh::_init()
{
    /* initialize cube vertices */

    vertices.push_back(Vec3d(-1, -1, -1)); // 1
    vertices.push_back(Vec3d(-1,  1, -1)); // 2
    vertices.push_back(Vec3d( 1,  1, -1)); // 3
    vertices.push_back(Vec3d( 1, -1, -1)); // 4
    vertices.push_back(Vec3d( 1,  1,  1)); // 5
    vertices.push_back(Vec3d( 1, -1,  1)); // 6
    vertices.push_back(Vec3d(-1,  1,  1)); // 7
    vertices.push_back(Vec3d(-1, -1,  1)); // 8

    /* initialize cube faces: 6 cube faces, 2 triangles per face == 12 elements
     * Each face has an associated texture coordinate, however, they could be draw just with their predefined color.
     *
     * Texture Coordinate origin:
     *
     * (u,v)
     *  0,0        1,0
     *   o -------->
     *   |
     *   |
     *   |
     *   v         o
     *  0,1       1,1
     */

    // front
    faces.push_back(Face(0, 1, 2, Tex2(0,1), Tex2(0,0), Tex2(1,0), 0xFFFFFFFF)); // 0xFFFF0000
    faces.push_back(Face(0, 2, 3, Tex2(0,1), Tex2(1,0), Tex2(1,1), 0xFFFFFFFF)); // 0xFFFF0000

    // right
    faces.push_back(Face(3, 2, 4, Tex2(0,1), Tex2(0,0), Tex2(1,0), 0xFFFFFFFF)); // 0xFF00FF00
    faces.push_back(Face(3, 4, 5, Tex2(0,1), Tex2(1,0), Tex2(1,1), 0xFFFFFFFF)); // 0xFF00FF00

    // back
    faces.push_back(Face(5, 4, 6, Tex2(0,1), Tex2(0,0), Tex2(1,0), 0xFFFFFFFF)); // 0xFF0000FF
    faces.push_back(Face(5, 6, 7, Tex2(0,1), Tex2(1,0), Tex2(1,1), 0xFFFFFFFF)); // 0xFF0000FF

    // left
    faces.push_back(Face(7, 6, 1, Tex2(0,1), Tex2(0,0), Tex2(1,0), 0xFFFFFFFF)); // 0xFFFFFF00
    faces.push_back(Face(7, 1, 0, Tex2(0,1), Tex2(1,0), Tex2(1,1), 0xFFFFFFFF)); // 0xFFFFFF00

    // top
    faces.push_back(Face(1, 6, 4, Tex2(0,1), Tex2(0,0), Tex2(1,0), 0xFFFFFFFF)); // 0xFF00FFFF
    faces.push_back(Face(1, 4, 2, Tex2(0,1), Tex2(1,0), Tex2(1,1), 0xFFFFFFFF)); // 0xFF00FFFF

    // bottom
    faces.push_back(Face(5, 7, 0, Tex2(0,1), Tex2(0,0), Tex2(1,0), 0xFFFFFFFF)); // 0xFFFF00FF
    faces.push_back(Face(5, 0, 3, Tex2(0,1), Tex2(1,0), Tex2(1,1), 0xFFFFFFFF)); // 0xFFFF00FF
}
