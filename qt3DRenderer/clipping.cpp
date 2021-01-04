#include "clipping.h"


Plane::Plane()
{
}

// lerp: performans linear interpolation between A and B using an interpolation factor T (range 0.0f-1.0f)
inline float lerp(const float& a, const float& b, const float& t)
{
    // in other words, when T=0 return A; When T=1.0 return B
    return a + t * (b - a);
}

Polygon::Polygon(const Vec3d& v1, const Vec3d& v2, const Vec3d& v3, const Tex2& t1, const Tex2& t2, const Tex2& t3)
{
    vertices = { v1, v2, v3 };
    texCoords = { t1, t2, t3 };
}

void Polygon::clip(const Plane frustumPlanes[6])
{
//    std::cout << "* Polygon::clip LEFT" << std::endl;
    _clipAgainstPlane(frustumPlanes[FRUSTUM_PLANE::LEFT]);

//    std::cout << "* Polygon::clip RIGHT" << std::endl;
    _clipAgainstPlane(frustumPlanes[FRUSTUM_PLANE::RIGHT]);

//    std::cout << "* Polygon::clip TOP" << std::endl;
    _clipAgainstPlane(frustumPlanes[FRUSTUM_PLANE::TOP]);

//    std::cout << "* Polygon::clip BOTTOM" << std::endl;
    _clipAgainstPlane(frustumPlanes[FRUSTUM_PLANE::BOTTOM]);

//    std::cout << "* Polygon::clip NEAR" << std::endl;
    _clipAgainstPlane(frustumPlanes[FRUSTUM_PLANE::NEAR]);

//    std::cout << "* Polygon::clip FAR" << std::endl;
    _clipAgainstPlane(frustumPlanes[FRUSTUM_PLANE::FAR]);
}

void Polygon::_clipAgainstPlane(const Plane& frustumPlane)
{
//    std::cout << "_clipAgainstPlane: point=" << frustumPlane.point << "  normal=" << frustumPlane.normal << std::endl;

    // the array of inside vertices that will be part of the final polygon returned via parameter
    std::vector<Vec3d> insideVertices;

    // the array that stored the associated texture coordinates
    std::vector<Tex2> insideTexCoords;

    if (!vertices.size())
        return;

    // if the current vertex is inside (the plane) and the previous is outside, must find the intersection point between to clip them
    Vec3d* curVertex = &vertices[0];
    Vec3d* prevVertex = &vertices[vertices.size()-1];
    Tex2* curTexCoord = &texCoords[0];
    Tex2* prevTexCoord = &texCoords[texCoords.size()-1];

    /*    ñ
     * . /
     *  º.       Q2    Q3
     *  P `.     *     *
     *      `.  /
     *        `o I
     *       /  `.       * Q4
     *      *     `.
     *     Q1       `.
     *             *  `.
     *            Q5    `.
     *                    `.
     *            OUTSIDE   `.  INSIDE (curDot > 0)
     */
    
    // calculate the current and previous dot-product: if curDot is positive, curVertex is inside the plane
    float curDot = 0;                                                               // dotQ1 = planeNormal . (Q1 - P)
    float prevDot = (*prevVertex - frustumPlane.point).dot(frustumPlane.normal);    // dotQ2 = planeNormal . (Q2 - P)
    
    // navigate through all the vertices: loop while the current is different than the last
    Vec3d* lastVertex = &vertices[vertices.size()-1]; // actually this is beyond the vector
    while (curVertex <= lastVertex)
    {
        curDot = (*curVertex - frustumPlane.point).dot(frustumPlane.normal);
//        std::cout << "_clipAgainstPlane  curDot=" << curDot << "  prevDot=" << prevDot << std::endl;

        // check if there was a change from "inside" to "outside" (or vice-versa) and calculate the intersection point (I)
        if (curDot * prevDot < 0)
        {
//            std::cout << "_clipAgainstPlane calculate intersection point (ADD to insideVertices)" << std::endl;

            // calculate interpolation factor t:    t = dotQ1 / (dotQ1 - dotQ2)
            float t = prevDot / (prevDot - curDot);

            // calculate intersection point I:      I = Q1 + t(Q2 - Q1)
            Vec3d intersectionPoint = (*curVertex - *prevVertex) * t + (*prevVertex);

            // save the intersection point in the list that stores the "inside vertices"
            insideVertices.push_back(intersectionPoint);

            // also calculate the intersection point for the texture coordinate using float LERP (linear interpolation)
            Tex2 interpolatedTexCoord;
            interpolatedTexCoord.u = lerp(prevTexCoord->u, curTexCoord->u, t);
            interpolatedTexCoord.v = lerp(prevTexCoord->v, curTexCoord->v, t);
            insideTexCoords.push_back(interpolatedTexCoord);
        }

        // check if the current point is inside the plane
        if (curDot > 0)
        {
//            std::cout << "_clipAgainstPlane vertex is INSIDE the plane (ADD to insideVertices)"<< std::endl;

            // save the current vertex in the list that stores the "inside vertices"
            insideVertices.push_back(*curVertex);

            // save the current texture coordinate in the proper list
            insideTexCoords.push_back(*curTexCoord);
        }

        // advance to the next vertex
        prevDot = curDot;
        prevVertex = curVertex;
        curVertex++;

        // advance to the next texture coordinate
        prevTexCoord = curTexCoord;
        curTexCoord++;

//        std::cout << std::endl;
    }

    // update the current polygon with only the vertices that are inside the plane
    vertices = insideVertices;

//    std::cout << "_clipAgainstPlane insideVertices sz=" << insideVertices.size() << std::endl;
//    std::cout << "_clipAgainstPlane num_vertices=" << vertices.size() << std::endl;
//    std::cout << "--------------------------------------------------------" << std::endl;

    // update the texture coordinates as well
    texCoords = insideTexCoords;
}

std::vector<Triangle> Polygon::triangles()
{
    std::vector<Triangle> triangles;

    if (vertices.size() < 3)
        return triangles;

    int idx0 = 0, idx1 = 0, idx2 = 0;

    for (unsigned int i = 0; i < vertices.size()-2; ++i)
    {
        // 3 indexes for the 3 vertices of the destination triangle
        idx1 = i + 1;
        idx2 = i + 2;

        Triangle triangle(Vec3d::toVec4d(vertices[idx0]), Vec3d::toVec4d(vertices[idx1]), Vec3d::toVec4d(vertices[idx2]),
                          texCoords[idx0], texCoords[idx1], texCoords[idx2]);

        triangles.push_back(triangle);
    }

    return triangles;
}
