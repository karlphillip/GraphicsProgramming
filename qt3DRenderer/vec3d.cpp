#include "vec3d.h"
#include "vec4d.h"

#include <cmath>


Vec3d::Vec3d()
{
    x = y = z = 0;
}

Vec3d::Vec3d(const float& x, const float& y, const float& z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

/* Vector Rotation
 *
 * | cos(a)  -sin(a) | * | x |      x' = x*cos(a) - y*sin(a)
 * | sin(a)   cos(a) |   | y |      y' = y*cos(a) + x*sin(a)
 */

// rotation around the Z axis
Vec3d Vec3d::rotateZ(const float& angle)
{
    Vec3d rotated(x * cos(angle) - y * sin(angle),
                  x * sin(angle) + y * cos(angle),
                  z);

    return rotated;
}

// rotation around the Y axis
Vec3d Vec3d::rotateY(const float& angle)
{
    Vec3d rotated(x * cos(angle) + z * sin(angle),
                  y,
                 -x * sin(angle) + z * cos(angle));

    return rotated;
}

// rotation around the X axis
Vec3d Vec3d::rotateX(const float& angle)
{
    Vec3d rotated(x,
                  y * cos(angle) - z * sin(angle),
                  y * sin(angle) + z * cos(angle));

    return rotated;
}

/* Angle Addition Property Proof
 * sin(a+B) = sin(a) * cos(B) + cos(a) * sin(B)
 * cos(a+B) = cos(a) * cos(B) - sin(a) * sin(B)
 */


float Vec3d::mag()
{
    // pythagoras theorem
    return std::sqrt(x*x + y*y + z*z);
}

Vec3d Vec3d::add(const Vec3d& v)
{
    return Vec3d(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vec3d Vec3d::sub(const Vec3d& v)
{
    return Vec3d(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vec3d Vec3d::mul(const float& factor)
{
    return Vec3d(this->x * factor, this->y * factor, this->z * factor);
}

Vec3d Vec3d::div(const float& factor)
{
    return Vec3d(this->x / factor, this->y / factor, this->z / factor);
}

/* cross product: what is the perpendicular vector between 2 vectors?
 *
 *                   .o  A
 *                 ' '
 *               .  .
 *             .   .
 *           .    .
 *         .     .
 *       .      .
 * C   o . . . o   B
 *
 * Vector AB is given by the sides: B - A
 * Vector AC is given by the sides: C - A
 *
 * To calculate the cross product between them, do:
 *                (B - A) x (C - A)
 *
 * It gives a vector perpendicular (90º) to AB and AC.
 *
 * The equation (B - A) x (C - A) only works because:
 *  - this renderer uses a Left-Handed Coordinate System (LHCS)
 *  - the vertices are defined clockwise
 *
 *         CLOCKWISE: A/B/C
 *  COUNTERCLOCKWISE: A/C/B
 *
 *              B
 *            . o
 *         .´  '
 *   A o.´    '
 *       `.  '
 *         `o
 *          C
 */
Vec3d Vec3d::cross(const Vec3d& v)
{
    return Vec3d(this->y * v.z - this->z * v.y,
                 this->z * v.x - this->x * v.z,
                 this->x * v.y - this->y * v.x);
}

/* dot product: how aligned are two vectors?
 *
 *           b
 *         /.
 *       /  .
 *     /    .
 *   /      .
 * o ======= - - - -> a
 *
 * Projection of B into A
 *
 * Curiosity:
 *  - if both vectors are 90º from each other, their dot product is ZERO
 *  - if both vectors are 180º from each other, their dot product is -1.0
 */
float Vec3d::dot(const Vec3d& v)
{
    return (this->x * v.x) + (this->y * v.y) + (this->z * v.z);
}


void Vec3d::norm()
{
    float magnitude = mag();
    this->x /= magnitude;
    this->y /= magnitude;
    this->z /= magnitude;
}

Vec4d Vec3d::toVec4d(const Vec3d& v)
{
    return Vec4d(v.x, v.y, v.z, 1.f);
}
