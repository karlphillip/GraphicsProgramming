/* matrix multiplication (multiply row by column)
 *
 * | 1  2 |  *  | 5 6 |  =  | (1+5)+(2*7)   (1*6)+(2*8) |
 * | 3  4 |     | 7 8 |     | (3*5)+(4*7)   (3*6)+(4*8) |
 *
 *                       =  | 19 22 |
 *                          | 43 50 |
 *
 * Properties of matrix mult:
 *  - Only allowed when the num cols of the Left matrix is equal to the num rows of the Right matrix: NxM * MxP = NxP
 *  - Its not commutative: A*B != B*A
 *  - Identify Matrix: a matrix multiplied by the I.M. returns the original matrix unchanged
 *
 * Rotation of 2D point (x,y) to point (x',y') by an angle α:
 *           x' = x*cosα - y*sinα
 *           y' = x*sinα + y*cosα
 *
 * The same equation appears in a Rotation Matrix:
 *      |x'|  =  | cosα   -sinα |  *  |x|
 *      |y'|     | sinα    cosα |     |y|
 *
 * We use 4x4 matrices for 3D transformations instead of 3x3 because Translation requires an extra row/column
 * to be performed. Also, to enable matrix-vector multiplication, an extra component W is added to the original
 * vector (think of W as being 1):
 *      | m m m m |   | x |
 *      | m m m m | * | y |
 *      | m m m m |   | z |
 *      | m m m m |   | 1 |
 */
#pragma once
//#include "vec3d.h"
#include <iostream>
#include <iomanip>

class Vec3d;

/* A 4x4 matrix class that can be used for linear transformations.
 */
class Mat4
{
public:
    Mat4();    

    // return the identity matrix
    static Mat4 eye();

    static Mat4 scale(const float& sx, const float& sy, const float& sz);

    static Mat4 translate(const float& tx, const float& ty, const float& tz);

    static Mat4 rotateX(const float& angle);

    static Mat4 rotateY(const float& angle);

    static Mat4 rotateZ(const float& angle);

    static Mat4 perspective(const float& fov, const float& aspect, const float& znear, const float& zfar);

    static Mat4 lookAt(Vec3d eye, Vec3d target, Vec3d up);

    // multiplication of Mat4 by Mat4
    Mat4 mul(const Mat4& m);

    // overload multiplication operator to execute mul()
    Mat4 operator*(const Mat4& m)
    {
        return mul(m);
    }

    // overload insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Mat4& v)
    {
        std::cout.precision(4);
        os << "Mat4=( " <<  std::setw(5) << v.m[0][0] << " " << std::setw(5) << v.m[0][1] << " " << std::setw(5) << v.m[0][2] << " " << std::setw(5) << v.m[0][3] << "\n" <<
              "       " <<  std::setw(5) << v.m[1][0] << " " << std::setw(5) << v.m[1][1] << " " << std::setw(5) << v.m[1][2] << " " << std::setw(5) << v.m[1][3] << "\n" <<
              "       " <<  std::setw(5) << v.m[2][0] << " " << std::setw(5) << v.m[2][1] << " " << std::setw(5) << v.m[2][2] << " " << std::setw(5) << v.m[2][3] << "\n" <<
              "       " <<  std::setw(5) << v.m[3][0] << " " << std::setw(5) << v.m[3][1] << " " << std::setw(5) << v.m[3][2] << " " << std::setw(5) << v.m[3][3] << " )";
        return os;
    }

    float m[4][4];
};

