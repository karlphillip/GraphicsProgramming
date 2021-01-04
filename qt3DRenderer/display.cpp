// Left-handed coordinate system: the Z value grows (+) towards the monitor.
#include "display.h"
#include "tex2.h"

#include <cmath>
#include <utility>
#include <QThread>

#define ORTHO 0


bool USE_PAINTERS_ALGO = false;


Display::Display()
{
    _colorBuffer = nullptr;
    _depthBuffer = nullptr;

    _screenWidth = _screenHeight = 0;
    _bkgColor = 0xFFFFFFFF; // black
    _fovFactor = 640;
}

Display::~Display()
{
    if (_colorBuffer)
        delete[] _colorBuffer;

    if (_depthBuffer)
        delete[] _depthBuffer;
}

void Display::setSize(const int& width, const int& height)
{
    std::cout << "Display::setSize: " << width << "x" << height << std::endl;

    if ((width >= 0 && height >= 0) && (width != _screenWidth || height != _screenHeight))
    {
        _screenWidth = width;
        _screenHeight = height;

        // recreate the Color Buffer with the appropriate size
        setup();
    }
}

int Display::width()
{
    return _screenWidth;
}

int Display::height()
{
    return _screenHeight;
}

void Display::setup()
{
    std::cout << "Display::setup" << std::endl;

    if (_colorBuffer)
        delete[] _colorBuffer;

    if (_depthBuffer)
        delete[] _depthBuffer;

    // allocate color buffer
    _colorBuffer = new uint32_t[_screenWidth * _screenHeight];

    // allocate z-buffer
    _depthBuffer = new float[_screenWidth * _screenHeight];

    // clear with solid color
    clearColorBuffer(_bkgColor);
    clearDepthBuffer(1.0f); // depth values range from 0.0f (near) to 1.0f (far)
}

// clearColorBuffer: input color is ARGB
void Display::clearColorBuffer(const uint32_t& c)
{
    for (int y = 0; y < _screenHeight; ++y)
        for (int x = 0; x < _screenWidth; ++x)
            _colorBuffer[_screenWidth*y+x] = c;
}

// clearDepthBuffer: input color is ARGB
void Display::clearDepthBuffer(const float& d)
{
    for (int y = 0; y < _screenHeight; ++y)
        for (int x = 0; x < _screenWidth; ++x)
            _depthBuffer[_screenWidth*y+x] = d;
}

uint32_t* Display::colorBuffer()
{
    return _colorBuffer;
}

float* Display::depthBuffer()
{
    return _depthBuffer;
}

void Display::drawGrid()
{
    //std::cout << "Display::drawGrid" << std::endl;

    unsigned int cellSize = 25;

    for (int y = 0; y < _screenHeight; y += cellSize)
        for (int x = 0; x < _screenWidth; x+= cellSize)
                _colorBuffer[_screenWidth*y+x] = 0xFF808080; // gray
}

#if ORTHO
Vec2d Display::project(Vec3d p)
{
    // the bigger the Z values, the more they divide
    return Vec2d(p.x * _fovFactor,
                 p.y * _fovFactor);
}
#else
// Weak Perspective Projection
Vec2d Display::project(Vec3d p)
{
    // the bigger the Z values, the more they divide
    return Vec2d(p.x * _fovFactor / p.z,    // Perspective Divide: P'x = Px / Pz
                 p.y * _fovFactor / p.z);   // Perspective Divide: P'y = Py / Pz
}
#endif

void Display::drawPixel(const int& x, const int& y, const uint32_t& color)
{
    //std::cout << "drawPixel: x=" << x << " y=" << y << " color=0x" << std::hex << color << std::dec << std::endl;

    if (x < 0 || x >= _screenWidth || y < 0 || y >= _screenHeight)
        return;

    // set pixel at row 10 column 20 to red: _screenWidth*10+20
    _colorBuffer[_screenWidth*y+x] = color;
}

void Display::drawPixel(const int& x, const int& y, const Vec4d& a, const Vec4d& b, const Vec4d& c, const uint32_t& color)
{
    // define the center of the baricentric coordinate computation and retrieve the weights at this position
    Vec2d p(x, y);
    Vec3d weights = _barycentricWeights(Vec4d::toVec2d(a), Vec4d::toVec2d(b), Vec4d::toVec2d(c), p);

    //std::cout << "------------------------------------------------------------" << std::endl;
    //std::cout << "drawPixel:  xy @ " << x << "," << y << "  a=" << a << " b=" << b << " c=" << c << " p=" << p << std::endl;

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    //std::cout << "drawPixel: alpha=" << alpha << " beta=" << beta << " gamma=" << gamma << std::endl;

    // Check if the values of alpha beta and gamma are valid
    const float EPSILON = 0.0000001f;
    if (alpha < -EPSILON || beta < -EPSILON || gamma < -EPSILON)
        return;

    float interpolated_reciprocal_w = 0;

    // TODO: calculate interpolated_reciprocal_w before drawPixel() so it is calculated only once per triangle instead of for every pixel

    // interpolate the value of 1/w for the current pixel
    interpolated_reciprocal_w = (1.f / a.w) * alpha + (1.f / b.w) * beta + (1.f / c.w) * gamma;

    /* draw the pixel only of the depth value is less than what's already stored in the depth buffer.
     * Keep in mind that because the reciprocal is being calculated, the closer a vertex is to the camera
     * the higher its 1/w value is going to be (i.e. 0.25). The furthest a vertex is, the smaller its 1/w is going to be (i.e. 0.17).
     * Adjust interpolated reciprocal of w to compensate for that:
     */
    interpolated_reciprocal_w = 1.0f - interpolated_reciprocal_w;
    int bufferIdx = (_screenWidth * y) + x;
    bufferIdx = bufferIdx % (_screenWidth * _screenHeight);

    if (interpolated_reciprocal_w < _depthBuffer[bufferIdx])
    {
        // draw the pixel
        drawPixel(x, y, color);

        // update z-buffer with this pixel's 1/w
        _depthBuffer[bufferIdx] = interpolated_reciprocal_w;
    }
}

/* Return the barycentric weights (alpha, beta, gamma) for point P inside triangle ABC.
 * An alternative to texture interpolation.
 *
 *                 A
 *                 o
 *                /|`.
 *               / |  `.
 *              /𝛾 |   β`.
 *             / .(P).    `.
 *         B  o´      ` .   `.
 *             `---_   α ` .  `.
 *                   ---_   ` . `.
 *                        ---_  `.`.
 *                             ---_`o  C
 */
Vec3d Display::_barycentricWeights(const Vec2d& a, const Vec2d& b, const Vec2d& c, const Vec2d& p)
{
    // find the vectors between vertices ABC and point P
    Vec2d ab = b.sub(a);
    Vec2d ac = c.sub(a);
    Vec2d bc = c.sub(b);

    Vec2d ap = p.sub(a);
    Vec2d bp = p.sub(b);

    // calculate the area of the ABC triangle using cross product (area of paralellogram)
    float triangleABCarea = (ab.x * ac.y - ab.y * ac.x);

    // weight alpha is the area of the subtriangle BCP divided by the area of the (full) triangle ABC
    float alpha = (bc.x * bp.y - bp.x * bc.y) / triangleABCarea;

    // weight beta is the area of the subtriangle ACP divided by the area of the (full) triangle ABC
    float beta = (ap.x * ac.y - ac.x * ap.y) / triangleABCarea;

    // weight gamma is easily found since barycentric coordinates always add up to 1
    float gamma = 1 - alpha - beta;

    Vec3d weights(alpha, beta, gamma);
    return weights;
}

void Display::drawTexel(const int& x, const int& y,
                        const Vec4d& a, const Vec4d& b, const Vec4d& c,
                        const Tex2& a_uv, const Tex2& b_uv, const Tex2& c_uv,
                        const uint32_t* texture, const int& textureWidth, const int& textureHeight,
                        const bool& fixDistortion)
{
    // define the center of the baricentric coordinate computation and retrieve the weights at this position
    Vec2d p(x, y);
    Vec3d weights = _barycentricWeights(Vec4d::toVec2d(a), Vec4d::toVec2d(b), Vec4d::toVec2d(c), p);

    //std::cout << "------------------------------------------------------------" << std::endl;
    //std::cout << "drawTexel:  xy @ " << x << "," << y << "  a=" << a << " b=" << b << " c=" << c << " p=" << p << std::endl;
    //std::cout << "drawTexel: uv1 @ " << u1 << "," << v1 << "  uv2 @ " << u2 << "," << v2 << "  uv3 @ " << u3 << "," << v3 << std::endl;

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    //std::cout << "drawTexel: alpha=" << alpha << " beta=" << beta << " gamma=" << gamma << std::endl;

    // preventing texture buffer overflow: actually disabled this for the modulus operation at the end
//    const float EPS = 0.00001f;
//    if (alpha < EPS || beta < EPS || gamma < EPS)
//        return;

    float interpolated_u = 0;
    float interpolated_v = 0;
    float interpolated_reciprocal_w = 0;

    // TODO: calculate interpolated_reciprocal_w before drawTexel() so it is calculated only once per triangle instead of for every pixel
    if (!fixDistortion)
    {
        // calculate the interpolation of U,V coords using barycentric weights (with perspective distortion bug)
        interpolated_u = (a_uv.u * alpha) + (b_uv.u * beta) + (c_uv.u * gamma);
        interpolated_v = (a_uv.v * alpha) + (b_uv.v * beta) + (c_uv.v * gamma);
    }
    else
    {
        /* calculate the interpolation of U,V coords using barycentric weights (perspective distortion fixed)
         *      1. Use the reciprocal of all attributes (1/w)
         *      2. Interpolate over the triangle face
         *      3. Divide all the attributes by 1/w (undoes the perspective transform)
         */
        interpolated_u = (a_uv.u / a.w) * alpha + (b_uv.u / b.w) * beta + (c_uv.u / c.w) * gamma;
        interpolated_v = (a_uv.v / a.w) * alpha + (b_uv.v / b.w) * beta + (c_uv.v / c.w) * gamma;
        interpolated_reciprocal_w = (1.f / a.w) * alpha + (1.f / b.w) * beta + (1.f / c.w) * gamma;

        interpolated_u /= interpolated_reciprocal_w;
        interpolated_v /= interpolated_reciprocal_w;
    }

    //std::cout << "drawTexel: interpolated_u=" << interpolated_u << " interpolated_v=" << interpolated_v << std::endl;

    // map X,Y position to the full texture size
    int texX = std::abs((int)(interpolated_u * textureWidth));
    int texY = std::abs((int)(interpolated_v * textureHeight));
    int texIndex = ((textureWidth * texY) + texX) % (textureWidth * textureHeight);
    uint32_t color = (uint32_t) texture[texIndex];
    //printf("1D=%d stores=0x%08x\n", ((textureWidth * texY) + texX), texture[(textureWidth * texY)+texX]);
    //std::cout << "drawTexel: texcoord @ " << texX << "," << texY << " texColor=0x" << std::hex << color << std::dec << std::endl;


    // DEBUG: first pixel on the top-left of the 1st rendered face
    /*
    if (texIndex > 4090)
    {
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "drawTexel:  xy @ " << x << "," << y << "  a=" << a << " b=" << b << " c=" << c << " p=" << p << std::endl;
        std::cout << "drawTexel: uv1 @ " << u1 << "," << v1 << "  uv2 @ " << u2 << "," << v2 << "  uv3 @ " << u3 << "," << v3 << std::endl;
        std::cout << "drawTexel: alpha=" << alpha << " beta=" << beta << " gamma=" << gamma << std::endl;
        std::cout << "drawTexel: interpolated_u=" << interpolated_u << " interpolated_v=" << interpolated_v << std::endl;
        std::cout << "drawTexel: texcoord @ " << texX << "," << texY << "  texIndex=" << texIndex << "  texColor=0x" << std::hex << color << std::dec << std::endl;
        //QThread::currentThread()->msleep(30000);
    }
    */

    /* draw the pixel only of the depth value is less than what's already stored in the depth buffer.
     * Keep in mind that because the reciprocal is being calculated, the closer a vertex is to the camera
     * the higher its 1/w value is going to be (i.e. 0.25). The furthest a vertex is, the smaller its 1/w is going to be (i.e. 0.17).
     * Adjust interpolated reciprocal of w to compensate for that:
     */
    interpolated_reciprocal_w = 1.0f - interpolated_reciprocal_w;
    int bufferIdx = (_screenWidth * y) + x;
    bufferIdx = bufferIdx % (_screenWidth * _screenHeight);

    if (interpolated_reciprocal_w < _depthBuffer[bufferIdx])
    {
        // draw the pixel
        drawPixel(x, y, color);

        // update z-buffer with this pixel's 1/w
        _depthBuffer[bufferIdx] = interpolated_reciprocal_w;
    }
}

// DDA algorithm: Digital Diferential Analyser
void Display::drawLine(const int& x1, const int& y1, const int& x2, const int& y2, const uint32_t& color)
{
    //std::cout << "drawLine: x1,y1=" << x1 << "," << y1 << " x2,y2=" << x2 << "," << y2 << std::endl;

    // Line equation: y = mx + c
    //      m = slope, calculated as: Dy / Dx (opposite side of the triangle DIVIDED BY the adjacent side)
    //      c = y-intercept
    int dx = x2 - x1;
    int dy = y2 - y1;

    // longest side length: how much to run
    int sideLen = (std::abs(dx) >= std::abs(dy)) ? std::abs(dx) : std::abs(dy);

    // size of the increment step
    float xInc = dx / (float) sideLen;
    float yInc = dy / (float) sideLen;

    //std::cout << "drawLine: dx=" << dx << " dy=" << dy << " xInc=" << xInc << " yInc=" << yInc << std::endl;

    float curX = (float)x1;
    float curY = (float)y1;

    for (int i = 0; i <= sideLen; ++i)
    {
        //std::cout << "drawLine: round(curX)=" << std::round(curX) << " round(curY)=" << std::round(curY) << std::endl;
        drawPixel((int)std::round(curX), (int)std::round(curY), color);
        curX += xInc;
        curY += yInc;
    }
}

void Display::drawRect(const int& x, const int& y, const int& w, const int& h, const uint32_t& color)
{
    //std::cout << "Display::drawRect rect x=" << x << " y=" << y << " w=" << w << " h=" << h << std::endl;

    if (x < 0 || x >= _screenWidth || y < 0 || y >= _screenHeight)
        return;

    int curX = 0;
    int curY = 0;

    for (int i = 0; i < w; ++i)
        for (int j = 0; j < h; ++j)
        {
            //_colorBuffer[_screenWidth*(y+j)+(x+i)] = color;

            curX = x + i;
            curY = y + j;
            drawPixel(curX, curY, color);
        }
}

void Display::drawTriangle(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, const uint32_t& color)
{
    //std::cout << "Display::drawTriangle x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << " x3=" << x3 << " y3=" << y3 << std::endl;
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x1, y1, color);
}

/* fillTriangle: triangle rasterization (flat-bottom & flat-top approach)
 * Split the triangle in two, half flat-bottom and half flat-top
 *
 *              (x1, y1)
 *                 o
 *                / \
 *              /    \               FLAT BOTTOM
 *            /       \
 *   (x2,y2) o - - - - o (Mx,My)                            Mx = (x3-x1) (y2-y1)
 *           `.         \                                        -------------- + x1
 *             `.        \                                           (y3-y1)
 *               `.       \
 *                 `.      \
 *                   `.     \        FLAT TOP
 *                     `.    \
 *                       `.   \
 *                         `.  \
 *                           `. \
 *                             ` o (x3,y3)
 */
void Display::fillTriangle(Vec4d p1, Vec4d p2, Vec4d p3, const uint32_t& color)
{
    // convert X,Y's to integer to fix rounding issues that mess up drawing and later crash drawPixel()
    p1.x = (int)p1.x;
    p1.y = (int)p1.y;
    p2.x = (int)p2.x;
    p2.y = (int)p2.y;
    p3.x = (int)p3.x;
    p3.y = (int)p3.y;

    if (p1.y > p2.y)
    {
        std::swap(p1.y, p2.y);
        std::swap(p1.x, p2.x);
        std::swap(p1.z, p2.z);
        std::swap(p1.w, p2.w);
    }

    if (p2.y > p3.y)
    {
        std::swap(p2.y, p3.y);
        std::swap(p2.x, p3.x);
        std::swap(p2.z, p3.z);
        std::swap(p2.w, p3.w);
    }

    if (p1.y > p2.y)
    {
        std::swap(p1.y, p2.y);
        std::swap(p1.x, p2.x);
        std::swap(p1.z, p2.z);
        std::swap(p1.w, p2.w);
    }

    // identify vector points for barycentric coordinates computation in drawTexel()
    Vec4d a = p1;
    Vec4d b = p2;
    Vec4d c = p3;

    /* draw the upper part of the triangle (flat-bottom) */

    // retrieve the slopes of both legs of the upper triangle
    float invLeftSlope = 0; // Dx / Dy
    if ((int)(p2.y - p1.y) != 0)
        invLeftSlope = (p2.x-p1.x) / (float)std::abs(p2.y-p1.y);

    float invRightSlope = 0; // Dx / Dy
    if ((int)(p3.y - p1.y) != 0)
        invRightSlope = (p3.x-p1.x) / (float)std::abs(p3.y-p1.y);

    // if (y2-y1) is zero, don't do any of this
    if ((int)(p2.y - p1.y) != 0)
    {
        // loop through all the scanlines (top to bottom)
        int xStart = 0, xEnd = 0;
        for (int y = p1.y; y <= (int)p2.y; ++y)
        {
            xStart = p2.x + (int)(y - p2.y) * invLeftSlope;
            xEnd   = p1.x + (int)(y - p1.y) * invRightSlope;

            // rotation of the triangle might cause xEnd to be before the xStart: hence the swap below
            if (xEnd < xStart)
                std::swap(xEnd, xStart);

            for (int x = xStart; x <= xEnd; ++x)
            {
                // draw pixel using the desired color
                drawPixel(x, y, a, b, c, color);
            }
        }
    }

    /* draw the lower part of the triangle (flat-top) */

    // retrieve the slopes of both legs of the lower triangle
    invLeftSlope = 0; // Dx / Dy
    if ((int)(p3.y - p2.y) != 0)
        invLeftSlope = (p3.x-p2.x) / (float)std::abs(p3.y-p2.y);

    invRightSlope = 0; // Dx / Dy
    if ((int)(p3.y - p1.y) != 0)
        invRightSlope = (p3.x-p1.x) / (float)std::abs(p3.y-p1.y);

    // if (y3-y2) is zero, don't do scanlines
    if ((int)(p3.y - p2.y) != 0)
    {
        // loop through all the scanlines (top to bottom)
        int xStart = 0, xEnd = 0;
        for (int y = p2.y; y <= (int)p3.y; ++y)
        {
            xStart = p2.x + (int)(y - p2.y) * invLeftSlope;
            xEnd   = p1.x + (int)(y - p1.y) * invRightSlope;

            // rotation of the triangle might cause xEnd to be positioned before xStart: hence the swap below
            if (xEnd < xStart)
                std::swap(xEnd, xStart);

            for (int x = xStart; x <= xEnd; ++x)
            {
                // draw pixel using the desired color
                drawPixel(x, y, a, b, c, color);
            }
        }
    }
}

// Draw textured triangle using flat-top/flat-bottom method
void Display::drawTexturedTriangle(Vec4d p1, Vec4d p2, Vec4d p3,
                                   Tex2 uv1, Tex2 uv2, Tex2 uv3,
                                   const uint32_t* texture, const int& textureWidth, const int& textureHeight,
                                   const bool& fixDistortion)
{
    //std::cout << "Display::drawTexturedTriangle x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << " x3=" << x3 << " y3=" << y3 << std::endl;

    if (!texture)
    {
        std::cout << "Display::drawTexturedTriangle !!! NULL texture" << std::endl;
        return;
    }

    // convert X,Y's to integer to fix rounding issues that mess up drawing and later crash drawTexel()
    p1.x = (int)p1.x;
    p1.y = (int)p1.y;
    p2.x = (int)p2.x;
    p2.y = (int)p2.y;
    p3.x = (int)p3.x;
    p3.y = (int)p3.y;

    /* sort the vectices by y-coord in asceding order (y1 < y2 < y3) */

    if (p1.y > p2.y)
    {
        std::swap(p1.y, p2.y);
        std::swap(p1.x, p2.x);
        std::swap(p1.z, p2.z);
        std::swap(p1.w, p2.w);


        std::swap(uv1.u, uv2.u);
        std::swap(uv1.v, uv2.v);
    }

    if (p2.y > p3.y)
    {
        std::swap(p2.y, p3.y);
        std::swap(p2.x, p3.x);
        std::swap(p2.z, p3.z);
        std::swap(p2.w, p3.w);

        std::swap(uv2.u, uv3.u);
        std::swap(uv2.v, uv3.v);
    }

    if (p1.y > p2.y)
    {
        std::swap(p1.y, p2.y);
        std::swap(p1.x, p2.x);
        std::swap(p1.z, p2.z);
        std::swap(p1.w, p2.w);

        std::swap(uv1.u, uv2.u);
        std::swap(uv1.v, uv2.v);
    }

    // flip the V component to account for inverted UV-coordinate system from .obj file (V grows downwards because origin is at thetop-left)
    uv1.v = 1.f - uv1.v;
    uv2.v = 1.f - uv2.v;
    uv3.v = 1.f - uv3.v;

    // identify vector points for barycentric coordinates computation in drawTexel()
    Vec4d a = p1;
    Vec4d b = p2;
    Vec4d c = p3;

    /* draw the upper part of the triangle (flat-bottom)
     *
     *           (x1,y1) (u1,v1)
     *             o
     *            . `.
     *           .    `.
     * (x2,y2) o.-------`o (mx,my)
     * (u2,v2)   ` .      `.
     *               ` .    `.
     *                   ` .  `.
     *                       ` .`.
     *                           ` o (x3,y3)
     *                               (u3,v3)
     */

    // retrieve the slopes of both legs of the upper triangle
    float invLeftSlope = 0; // Dx / Dy
    if ((int)(p2.y - p1.y) != 0)
        invLeftSlope = (float)(p2.x-p1.x) / (float)std::abs(p2.y-p1.y);

    float invRightSlope = 0; // Dx / Dy
    if ((int)(p3.y - p1.y) != 0)
        invRightSlope = (float)(p3.x-p1.x) / (float)std::abs(p3.y-p1.y);

    // if (y2-y1) is zero, don't do any of this
    if ((int)(p2.y - p1.y) != 0)
    {
        // loop through all the scanlines (top to bottom)
        int xStart = 0, xEnd = 0;
        for (int y = p1.y; y <= p2.y; ++y)
        {
            xStart = (p2.x) + (y - p2.y) * invLeftSlope;
            xEnd   = (p1.x) + (y - p1.y) * invRightSlope;

            // rotation of the triangle might cause xEnd to be before the xStart: hence the swap below
            if (xEnd < xStart)
                std::swap(xEnd, xStart);

            for (int x = xStart; x <= xEnd; ++x)
            {
                // draw pixel using predefined color for testing
                //drawPixel(x, y, 0xFFFFFF00); // yellow

                // draw pixel using color from the texture
                drawTexel(x, y, a, b, c, uv1, uv2, uv3, texture, textureWidth, textureHeight, fixDistortion);
            }
        }
    }

    /* draw the lower part of the triangle (flat-top) */

    // retrieve the slopes of both legs of the lower triangle
    invLeftSlope = 0; // Dx / Dy
    if ((int)(p3.y - p2.y) != 0)
        invLeftSlope = (float)(p3.x-p2.x) / (float)std::abs(p3.y-p2.y);

    invRightSlope = 0; // Dx / Dy
    if ((int)(p3.y - p1.y) != 0)
        invRightSlope = (float)(p3.x-p1.x) / (float)std::abs(p3.y-p1.y);

    // if (y3-y2) is zero, don't do scanlines
    if ((int)(p3.y - p2.y) != 0)
    {
        // loop through all the scanlines (top to bottom)
        int xStart = 0, xEnd = 0;
        for (int y = p2.y; y <= p3.y; ++y)
        {
            xStart = p2.x + (y - p2.y) * invLeftSlope;
            xEnd   = p1.x + (y - p1.y) * invRightSlope;

            // rotation of the triangle might cause xEnd to be positioned before xStart: hence the swap below
            if (xEnd < xStart)
                std::swap(xEnd, xStart);

            for (int x = xStart; x <= xEnd; ++x)
            {
                //std::cout << "Display::drawTexturedTriangle: flat-top x=" << 0 << " y=" << y << std::endl;

                // draw pixel using color from the texture
                //drawPixel(x, y, 0xFFFF00FF); // magenta

                // draw pixel using color from the texture
                drawTexel(x, y, a, b, c, uv1, uv2, uv3, texture, textureWidth, textureHeight, fixDistortion);
            }
        }
    }
}
