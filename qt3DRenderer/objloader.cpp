/*
 * mtllib cube.mtl
 * o cube
 *
 * v -1.000000 -1.000000 1.000000
 * v 1.000000 -1.000000 1.000000
 * v -1.000000 1.000000 1.000000
 * v 1.000000 1.000000 1.000000
 * v -1.000000 1.000000 -1.000000
 * v 1.000000 1.000000 -1.000000
 * v -1.000000 -1.000000 -1.000000
 * v 1.000000 -1.000000 -1.000000
 *
 * vt 1.000000 0.000000
 * vt 0.000000 0.000000
 * vt 1.000000 1.000000
 * vt 0.000000 1.000000
 *
 * vn 0.000000 0.000000 1.000000
 * vn 0.000000 1.000000 0.000000
 * vn 0.000000 0.000000 -1.000000
 * vn 0.000000 -1.000000 0.000000
 * vn 1.000000 0.000000 0.000000
 * vn -1.000000 0.000000 0.000000
 *
 * g cube
 * usemtl cube
 * s 1
 * f 1/1/1 2/2/1 3/3/1
 * f 3/3/1 2/2/1 4/4/1
 * s 2
 * f 3/1/2 4/2/2 5/3/2
 * f 5/3/2 4/2/2 6/4/2
 * s 3
 * f 5/4/3 6/3/3 7/2/3
 * f 7/2/3 6/3/3 8/1/3
 * s 4
 * f 7/1/4 8/2/4 1/3/4
 * f 1/3/4 8/2/4 2/4/4
 * s 5
 * f 2/1/5 8/2/5 4/3/5
 * f 4/3/5 8/2/5 6/4/5
 * s 6
 * f 7/1/6 1/2/6 5/3/6
 * f 5/3/6 1/2/6 3/4/6
 */
#include "objloader.h"
#include "tex2.h"

#include <stdio.h>
#include <string.h>

#include <iostream>


OBJLoader::OBJLoader(const std::string& filename)
{
    FILE* file = fopen(filename.c_str(), "r");
    if (!file)
    {
        std::cout << "!!! OBJLoader: unable to open file " << filename << std::endl;
        std::cout << "Have you updated ASSETS_DIR in window.cpp?" << std::endl;
        exit(-1);
    }

    // store the texture coordinates from the vt section
    std::vector<Tex2> texCoords;

    char line[1024];
    while (fgets(line, 1024, file))
    {
        // vertex data
        if (strncmp(line, "v ", 2) == 0)
        {
            Vec3d vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);                                // v -1.000000 -1.000000 1.000000
            _mesh.vertices.push_back(vertex);
        }

        // texture coordinates info
        if (strncmp(line, "vt ", 3) == 0)
        {
            Tex2 texCoord;
            sscanf(line, "vt %f %f", &texCoord.u, &texCoord.v);
            texCoords.push_back(texCoord);
        }

        /* f 5/4/3 6/3/3 7/2/3
         *  - each one of these 3 sections describe the vertexIdx/texCoordIdx/NormalIdx for one vertex of the triangular face
         *  - number 5: the first number. Its the 5th vertex index (stored in the v section) for the first vertex of this triangular face
         *  - number 4: the second number. Its the 4th texture coord index (stored in the vt section) associated to this vertex
         */
        if (strncmp(line, "f ", 2) == 0)
        {
            int vertexIdx[3];
            int textureIdx[3];
            int normalsIdx[3];
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertexIdx[0], &textureIdx[0], &normalsIdx[0], // f 1/1/1 2/2/1 3/3/1
                                                         &vertexIdx[1], &textureIdx[1], &normalsIdx[1],
                                                         &vertexIdx[2], &textureIdx[2], &normalsIdx[2]);

            // store the 3 vertices of the face and its associated texture coord info
            Face face(vertexIdx[0]-1, vertexIdx[1]-1, vertexIdx[2]-1,
                      texCoords[textureIdx[0]-1], texCoords[textureIdx[1]-1], texCoords[textureIdx[2]-1],
                      0xFFFFFFFF);

            _mesh.faces.push_back(face);
        }
    }
}

Mesh OBJLoader::mesh()
{
    return _mesh;
}
