#include "mat4.h"
#include "vec3d.h"

Mat4::Mat4()
{
    m[0][0] = 0; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0; // row 0
    m[1][0] = 0; m[1][1] = 0; m[1][2] = 0; m[1][3] = 0; // row 1
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 0; m[2][3] = 0; // row 2
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 0; // row 3
}

Mat4 Mat4::eye()
{
    Mat4 mat;
    mat.m[0][0] = 1; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
    mat.m[1][0] = 0; mat.m[1][1] = 1; mat.m[1][2] = 0; mat.m[1][3] = 0;
    mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = 1; mat.m[2][3] = 0;
    mat.m[3][0] = 0; mat.m[3][1] = 0; mat.m[3][2] = 0; mat.m[3][3] = 1;

    return mat;
}

Mat4 Mat4::scale(const float& sx, const float& sy, const float& sz)
{
    /* | sz  0  0  0  |
     * |  0 sy  0  0  |
     * |  0  0 sz  0  |
     * |  0  0  0  1  |
     */
    Mat4 mat = eye();
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;

    return mat;
}

Mat4 Mat4::translate(const float& tx, const float& ty, const float& tz)
{
    /* |  1  0  0  tx |
     * |  0  1  0  ty |
     * |  0  0  1  tz |
     * |  0  0  0  1  |
     */
    Mat4 mat = eye();
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;

    return mat;
}

Mat4 Mat4::rotateX(const float& angle)
{
    /* |  1  0  0  0  |
     * |  0  c -s  0  |
     * |  0  s  c  0  |
     * |  0  0  0  1  |
     *
     * To rotate around the X axis, leave the X row and X column untouched.
     */
    float c = std::cos(angle);
    float s = std::sin(angle);

    Mat4 mat = eye();
    mat.m[1][1] = c;
    mat.m[1][2] = -s;
    mat.m[2][1] = s;
    mat.m[2][2] = c;
    return mat;
}

Mat4 Mat4::rotateY(const float& angle)
{
    /* |  c  0  s  0  |
     * |  0  1  0  0  |
     * | -s  0  c  0  |
     * |  0  0  0  1  |
     *
     * To rotate around the Y axis, leave the Y row and Y column untouched.
     */
    float c = std::cos(angle);
    float s = std::sin(angle);

    Mat4 mat = eye();
    mat.m[0][0] = c;
    mat.m[0][2] = s;
    mat.m[2][0] = -s;
    mat.m[2][2] = c;
    return mat;
}

Mat4 Mat4::rotateZ(const float& angle)
{
    /* |  c -s  0  0  |
     * |  s  c  0  0  |
     * |  0  0  1  0  |
     * |  0  0  0  1  |
     *
     * To rotate around the Z axis, leave the Z row and Z column untouched.
     */
    float c = std::cos(angle);
    float s = std::sin(angle);

    Mat4 mat = eye();
    mat.m[0][0] = c;
    mat.m[0][1] = -s;
    mat.m[1][0] = s;
    mat.m[1][1] = c;
    return mat;
}

Mat4 Mat4::mul(const Mat4& mat)
{
    Mat4 result;

    for (int i = 0; i < 4; ++i) // i: row
    {
        for (int j = 0; j < 4; ++j) // j : column
        {
            result.m[i][j] = this->m[i][0] * mat.m[0][j] + this->m[i][1] * mat.m[1][j] + this->m[i][2] * mat.m[2][j] + this->m[i][3] * mat.m[3][j];
        }
    }

    return result;
}


/* Strong Perspective Projection
 *
 * [1]   a = h / w                                              // a = aspect ratio
 * [2]   f = 1 / (tan(angle/2))                                 // f = field of view
 * [3,4] q = (zfar/(zfar-znear)) - ((zfar*znear)/(zfar-znear))  // q = normalized Z
 *
 * Conversion of a 3D point into Screen Space:
 * | x |     | a*f*x |
 * | y | --> |   f*y |
 * | z |     |   q*z |
 *
 * Assembled Projection Matrix:
 * | [1]*[2]    0       0       0   |
 * |   0       [2]      0       0   |
 * |   0        0      [3]     [4]  |
 * |   0        0       1       0   |
 */
Mat4 Mat4::perspective(const float& fov, const float& aspect, const float& znear, const float& zfar)
{
    Mat4 proj;
    proj.m[0][0] = aspect * (1.f / std::tan(fov/2.f));
    proj.m[1][1] = 1.f / std::tan(fov/2.f);
    proj.m[2][2] = zfar / (zfar - znear);
    proj.m[2][3] = (-zfar * znear) / (zfar - znear);
    proj.m[3][2] = 1.f;

    return proj;
}

/* The Camera model Look-At returns a View Matrix and its made of two transformations:
 * Mview = Mr * Mt
 *
 *  1. Translate the whole scene inversely from the Camera Eye position to the origin. Replace the 4th column of the translation
 * matrix Mt by the negated Eye position (-EyeX, -EyeY, -EyeZ);
 *  2. Rotate the scene with reverse orientation so the camera is positioned at the origin
 * and facing positive Z-axis (LHCS) to look to the target. We must compute the Forward(Z) Right(X) and Up(Y) vectors.
 *
 *         | Xx    Xy    Xz   (-Xx*EyeX-Xy*EyeY-Xz*EyeZ) |
 * Mview = | Yx    Yy    Yz   (-Ux*EyeX-Uy*EyeY-Uz*EyeZ) |
 *         | Zx    Zy    Zz   (-Zx*EyeX-Zy*EyeY-Zz*EyeZ) |
 *         | 0     0     0                1              |
 *
 * which is equivalent to:
 *         | Xx    Xy    Xz   -dot(X,eye) |
 * Mview = | Yx    Yy    Yz   -dot(Y,eye) |
 *         | Zx    Zy    Zz   -dot(Z,eye) |
 *         | 0     0     0         1      |
 */
Mat4 Mat4::lookAt(Vec3d eye, Vec3d target, Vec3d up)
{
    Vec3d z = target.sub(eye);  // forward (Z) vector
    z.norm();

    Vec3d x = up.cross(z);      // right (X) vector
    x.norm();

    Vec3d y = z.cross(x);       // up (Y) vector

    /* | x.x    x.y    x.z    -Vec3d::dot(x, eye) |
     * | y.x    y.y    y.z    -Vec3d::dot(y, eye) |
     * | z.x    z.y    z.z    -Vec3d::dot(z, eye) |
     * |  0      0      0              1          |
     */
    Mat4 viewMatrix = Mat4::eye(); // load identity matrix
    viewMatrix.m[0][0] = x.x;     viewMatrix.m[0][1] = x.y;     viewMatrix.m[0][2] = x.z;     viewMatrix.m[0][3] = -x.dot(eye);
    viewMatrix.m[1][0] = y.x;     viewMatrix.m[1][1] = y.y;     viewMatrix.m[1][2] = y.z;     viewMatrix.m[1][3] = -y.dot(eye);
    viewMatrix.m[2][0] = z.x;     viewMatrix.m[2][1] = z.y;     viewMatrix.m[2][2] = z.z;     viewMatrix.m[2][3] = -z.dot(eye);

    return viewMatrix;
}
