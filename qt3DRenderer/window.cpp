/* This 3D renderer uses a Left-Handed Coordinate System (LHCS): the Z-value grows (positive) inside the monitor.
 *
 * Also, the winding order of a triangle is Clockwise:
 *
 *      CLOCKWISE: A/B/C           COUNTERCLOCKWISE: A/C/B
 *
 *              B                             B
 *            . o                          . o
 *         .´  '                        .´  '
 *   A o.´  / '                   A o.´    '
 *       `./ '                        `.  '
 *        /`o                           `o
 *       ñ  C                            C
 *
 * The Normal for a triangle in LHCS that points towards the camera would be:
 *      Vec3d(0.f, 0.f, -1.f)
 */
#include "vec4d.h"
#include "mat4.h"
#include "window.h"
#include "objloader.h"
#include "tex2.h"

#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QThread>

#define PI 3.14159265358979323846

#define WND_WIDTH 1280
#define WND_HEIGHT 900

#define FPS 200
#define RENDER_WAIT_MS (1000 / FPS)

#define ASSETS_DIR "C:\\Users\\karlp\\Documents\\workspace\\GraphicsProgramming\\qt3DRenderer\\assets"
#define WIREFRAME_COLOR 0xFFFFFFFF


// global flags
bool ENABLE_FACE_CULL       = true;
bool FIX_TEXTURE_DISTORTION = true;
bool ORBIT_CAMERA           = true;


// hex2argb: returns red 0xFF800000 as ARGB QColor(255, 128, 0, 0)
QColor hex2argb(const uint32_t& c)
{
    QColor out = QColor((uint8_t) ((c >> 24) & 0xFF),    // alpha
                        (uint8_t) ((c >> 16) & 0xFF),    // red
                        (uint8_t) ((c >> 8)  & 0xFF),    // green
                        (uint8_t) (c & 0xFF));           // blue

//    qDebug() << "hex2rgba a=" << out.alpha();
//    qDebug() << "hex2rgba r=" << out.red();
//    qDebug() << "hex2rgba g=" << out.green();
//    qDebug() << "hex2rgba b=" << out.blue();

    return out;
}

Window::Window()
:_width(WND_WIDTH), _height(WND_HEIGHT), _tick_ms(5)
{
    _deltaTime = 0.f;
    _prevTime = QDateTime::currentMSecsSinceEpoch();

    // resize window
    resize(_width, _height);

    // setup Display/Color Buffer
    _gfx.setSize(_width, _height);
    _gfx.setup();

    /* load 3D model and its texture: initialize _mesh with the vertices and faces of a 3D model */

    // load a hardcoded cube and a predefined texture
//    Mesh meshCube1 = CubeMesh((const uint32_t*)REDBRICK_TEXTURE, REDBRICK_WIDTH, REDBRICK_HEIGHT);
//    meshCube1.scale = Vec3d(1.f, 1.f, 1.f);
//    meshCube1.translation = Vec3d(-3.f, 0.f, 5.f);
//    _meshObjects.push_back(meshCube1);

    // load a cube mesh from an .obj file using a custom texture
//    QImage texImageCube = QImage(QString(ASSETS_DIR) + "\\cube.png").convertToFormat(QImage::Format_ARGB32);
//    Mesh meshCube2 = OBJLoader(std::string(ASSETS_DIR) + "\\cube.obj").mesh();
//    meshCube2.setTexture(reinterpret_cast<uint32_t*>(texImageCube.bits()), texImageCube.width(), texImageCube.height());
//    meshCube2.scale = Vec3d(1.f, 1.f, 1.f);
//    meshCube2.translation = Vec3d(+3.f, 0.f, 5.f);
//    _meshObjects.push_back(meshCube2);

    // load the Runway
    QImage texImageRunway = QImage(QString(ASSETS_DIR) + "\\runway.png").convertToFormat(QImage::Format_ARGB32);
    Mesh meshRunway = OBJLoader(std::string(ASSETS_DIR) + "\\runway.obj").mesh();
    meshRunway.setTexture(reinterpret_cast<uint32_t*>(texImageRunway.bits()), texImageRunway.width(), texImageRunway.height());
    meshRunway.scale = Vec3d(1.f, 1.f, 1.f);
    meshRunway.translation = Vec3d(0.f, -1.5f, 23.f);
    _meshObjects.push_back(meshRunway);

    // load the F22
    QImage texImageF22 = QImage(QString(ASSETS_DIR) + "\\f22.png").convertToFormat(QImage::Format_ARGB32);
    Mesh meshF22 = OBJLoader(std::string(ASSETS_DIR) + "\\f22.obj").mesh();
    meshF22.setTexture(reinterpret_cast<uint32_t*>(texImageF22.bits()), texImageF22.width(), texImageF22.height());
    meshF22.scale = Vec3d(1.f, 1.f, 1.f);
    meshF22.translation = Vec3d(0.f, -1.3f, 5.f);
    meshF22.rotation = Vec3d(0.f, -PI/2.f, 0.f); // rotate 90º
    _meshObjects.push_back(meshF22);

    // load the EFA aircraft
    QImage texImageEFA = QImage(QString(ASSETS_DIR) + "\\efa.png").convertToFormat(QImage::Format_ARGB32);
    Mesh meshEFA = OBJLoader(std::string(ASSETS_DIR) + "\\efa.obj").mesh();
    meshEFA.setTexture(reinterpret_cast<uint32_t*>(texImageEFA.bits()), texImageEFA.width(), texImageEFA.height());
    meshEFA.scale = Vec3d(1.f, 1.f, 1.f);
    meshEFA.translation = Vec3d(-2.f, -1.3f, 9.f);
    meshEFA.rotation = Vec3d(0.f, -PI/2.f, 0.f); // rotate 90º
    _meshObjects.push_back(meshEFA);

    // load the F117
    QImage texImageF117 = QImage(QString(ASSETS_DIR) + "\\f117.png").convertToFormat(QImage::Format_ARGB32);
    Mesh meshF117 = OBJLoader(std::string(ASSETS_DIR) + "\\f117.obj").mesh();
    meshF117.setTexture(reinterpret_cast<uint32_t*>(texImageF117.bits()), texImageF117.width(), texImageF117.height());
    meshF117.scale = Vec3d(1.f, 1.f, 1.f);
    meshF117.translation = Vec3d(+2.f, -1.3f, 9.f);
    meshF117.rotation = Vec3d(0.f, -PI/2.f, 0.f); // rotate 90º
    _meshObjects.push_back(meshF117);

    // load the Crab
//    QImage texImageCrab = QImage(QString(ASSETS_DIR) + "\\crab.png").convertToFormat(QImage::Format_ARGB32);
//    Mesh meshCrab = OBJLoader(std::string(ASSETS_DIR) + "\\crab.obj").mesh();
//    meshCrab.setTexture(reinterpret_cast<uint32_t*>(texImageCrab.bits()), texImageCrab.width(), texImageCrab.height());
//    meshCrab.scale = Vec3d(1.f, 1.f, 1.f);
//    meshCrab.translation = Vec3d(0.f, 2.f, 6.f);
//    _meshObjects.push_back(meshCrab);

    // load the Drone
//    QImage texImageDrone = QImage(QString(ASSETS_DIR) + "\\drone.png").convertToFormat(QImage::Format_ARGB32);// load a Drone
//    Mesh meshDrone = OBJLoader(std::string(ASSETS_DIR) + "\\drone.obj").mesh();
//    meshDrone.setTexture(reinterpret_cast<uint32_t*>(texImageDrone.bits()), texImageDrone.width(), texImageDrone.height());
//    meshDrone.scale = Vec3d(1.f, 1.f, 1.f);
//    meshDrone.translation = Vec3d(0.f, 0.f, 6.f);
//    _meshObjects.push_back(meshDrone);

    // initialize light source: in LHCS, Z grows positive towards inside the monitor (i.e. away from the camera)
    _lightSource.direction = Vec3d(0, 0, 1);

    // initialize camera position
    _camera.position = Vec3d(0, 0, 0);  // placed at the origin
    _camera.direction = Vec3d(0, 0, 1); // looking at positive Z-axis

    // define the initial orbit angle of the camera and its distance from the target
    _cameraOrbitAngle = 270.f;
    _cameraOrbitDistance = 7.f;

    // initialize default rendering mode
    _renderMode = RENDER_MODE::WIREFRAME; // TRIANGLES

    /* initialize projection matrix */

    float arX = _width / (float)_height;    // horizontal Aspect Ratio
    float arY = _height / (float)_width;    // vertical Aspect Ratio
    float fovY = (float)PI / 3.f;           // 60º is 180/3 which is equivalent to PI/3 in radians
    float fovX = std::atan(std::tan(fovY / 2.f) * arX) * 2;
    float zNear = 1.0f;
    float zFar = 100.f;
    _projMatrix = Mat4::perspective(fovY, arY, zNear, zFar);

    /* initialize frustum planes for Clipping operation */

    _initFrustumPlanes(fovX, fovY, zNear, zFar);

    /* start timer to draw frames */

    _timer = new QTimer();
    _timer->start(_tick_ms);
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(_tick()));    

    qDebug() << "Window::Window:           ORBIT_CAMERA=" << ORBIT_CAMERA;
    qDebug() << "Window::Window:       ENABLE_FACE_CULL=" << ENABLE_FACE_CULL;
    qDebug() << "Window::Window: FIX_TEXTURE_DISTORTION=" << FIX_TEXTURE_DISTORTION;
}

Window::~Window()
{
}

void Window::_tick()
{
    // trigger paint event to redraw the window
    update();
}

void Window::_renderColorBuffer(QPainter& p)
{
    //qDebug() << "Window::_renderColorBuffer";

    _framebuffer = QImage((const uchar*)(_gfx.colorBuffer()), _gfx.width(), _gfx.height(), QImage::Format_ARGB32);
//    if (!_framebuffer.save("framebuffer.jpg"))
//        qDebug() << "_renderColorBuffer!!! image";

    // scale 3D screen to the window size if necessary
    if (_gfx.width() != _width || _gfx.height() != _height)
    {
        _framebuffer = _framebuffer.scaled(_width, _height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }

    p.drawImage(QPoint(0, 0), _framebuffer);
}

/* Frustum planes are defined by a point and a normal vector
 */
void Window::_initFrustumPlanes(const float& fovX, const float& fovY, const float& zNear, const float& zFar)
{
//    printf("init_frustum_planes: fovX=%.2f  fovY=%.2f  z_near=%.2f  z_zfar=%.2f\n", fovX, fovY, zNear, zFar);

    float cosHalfFovX = std::cos(fovX/2.f);
    float sinHalfFovX = std::sin(fovX/2.f);
    float cosHalfFovY = std::cos(fovY/2.f);
    float sinHalfFovY = std::sin(fovY/2.f);

//    printf("init_frustum_planes: cosHalfFovX=%.2f  sinHalfFovX=%.2f\n", cosHalfFovX, sinHalfFovX);
//    printf("init_frustum_planes: cosHalfFovY=%.2f  sinHalfFovY=%.2f\n", cosHalfFovY, sinHalfFovY);

    Vec3d origin(0.f, 0.f, 0.f);

    _frustumPlanes[FRUSTUM_PLANE::LEFT].point = origin;
    _frustumPlanes[FRUSTUM_PLANE::LEFT].normal.x = cosHalfFovX;
    _frustumPlanes[FRUSTUM_PLANE::LEFT].normal.y = 0;
    _frustumPlanes[FRUSTUM_PLANE::LEFT].normal.z = sinHalfFovX;
//    std::cout << "init_frustum_planes: LEFT point=" << _frustumPlanes[FRUSTUM_PLANE::LEFT].point << "  normal=" << _frustumPlanes[FRUSTUM_PLANE::LEFT].normal << std::endl;

    _frustumPlanes[FRUSTUM_PLANE::RIGHT].point = origin;
    _frustumPlanes[FRUSTUM_PLANE::RIGHT].normal.x = -cosHalfFovX;
    _frustumPlanes[FRUSTUM_PLANE::RIGHT].normal.y = 0;
    _frustumPlanes[FRUSTUM_PLANE::RIGHT].normal.z = sinHalfFovX;
//    std::cout << "init_frustum_planes: RIGHT point=" << _frustumPlanes[FRUSTUM_PLANE::RIGHT].point << "  normal=" << _frustumPlanes[FRUSTUM_PLANE::RIGHT].normal << std::endl;

    _frustumPlanes[FRUSTUM_PLANE::TOP].point = origin;
    _frustumPlanes[FRUSTUM_PLANE::TOP].normal.x = 0;
    _frustumPlanes[FRUSTUM_PLANE::TOP].normal.y = -cosHalfFovY;
    _frustumPlanes[FRUSTUM_PLANE::TOP].normal.z = sinHalfFovY;
//    std::cout << "init_frustum_planes: TOP point=" << _frustumPlanes[FRUSTUM_PLANE::TOP].point << "  normal=" << _frustumPlanes[FRUSTUM_PLANE::TOP].normal << std::endl;

    _frustumPlanes[FRUSTUM_PLANE::BOTTOM].point = origin;
    _frustumPlanes[FRUSTUM_PLANE::BOTTOM].normal.x = 0;
    _frustumPlanes[FRUSTUM_PLANE::BOTTOM].normal.y = cosHalfFovY;
    _frustumPlanes[FRUSTUM_PLANE::BOTTOM].normal.z = sinHalfFovY;
//    std::cout << "init_frustum_planes: BOTTOM point=" << _frustumPlanes[FRUSTUM_PLANE::BOTTOM].point << "  normal=" << _frustumPlanes[FRUSTUM_PLANE::BOTTOM].normal << std::endl;

    _frustumPlanes[FRUSTUM_PLANE::NEAR].point = Vec3d(0.f, 0.f, zNear);
    _frustumPlanes[FRUSTUM_PLANE::NEAR].normal.x = 0;
    _frustumPlanes[FRUSTUM_PLANE::NEAR].normal.y = 0;
    _frustumPlanes[FRUSTUM_PLANE::NEAR].normal.z = 1;
//    std::cout << "init_frustum_planes: NEAR point=" << _frustumPlanes[FRUSTUM_PLANE::NEAR].point << "  normal=" << _frustumPlanes[FRUSTUM_PLANE::NEAR].normal << std::endl;

    _frustumPlanes[FRUSTUM_PLANE::FAR].point = Vec3d(0.f, 0.f, zFar);
    _frustumPlanes[FRUSTUM_PLANE::FAR].normal.x = 0;
    _frustumPlanes[FRUSTUM_PLANE::FAR].normal.y = 0;
    _frustumPlanes[FRUSTUM_PLANE::FAR].normal.z = -1;
//    std::cout << "init_frustum_planes: FAR point=" << _frustumPlanes[FRUSTUM_PLANE::FAR].point << "  normal=" << _frustumPlanes[FRUSTUM_PLANE::FAR].normal << std::endl;
}

void Window::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    qDebug() << "Window::resizeEvent: " << width() << "x" << height();

    if (width() != _width || height() != _height)
    {
        _width = width();
        _height = height();

        // setup color buffer again
        _gfx.setSize(_width, _height);

        // trigger paintEvent
        update();
    }
}

void Window::paintEvent(QPaintEvent* e)
{
    //qDebug() << "Window::paintEvent";

    if (!_gfx.colorBuffer())
        qDebug() << "paintEvent: null color buffer";

    QPainter painter(this);

    /* update: linear transforms, perspective projection */

    updt();

    /* render  */

    render(painter);

    QWidget::paintEvent(e);
}

/* _processGraphicsPipeline: passes a mesh through each state of the Graphics Pipeline:
 *
 * Current stages of the Graphics Pipeline:
 *  + Model Space: a 3D Mesh (vertices) starts in the Model Space (in its own original local coordinate system: Blender, Maya, ...)
 *
 *  + World Space: vertices from the previous stage are multiplied by the World Matrix to be in the World Space (scale, translation, rotation)
 *
 *  + Camera Space: vertices from the previous stag are multiplied by the View Matrix to be in View/Camera Space
 *    (the Eye becomes the new origin of the Camera and everything in the world is scaled/translated/rotated
 *     so that we see things from the Camera Eye point of view)
 *
 * + Backface Culling: a Hidden Surface Removal technique is executed in Camera Space to discard the faces that are looking
 *   away from the camera
 *
 * + Clipping: then Frustum Clipping is executed to get rid of the vertices that are not inside the 6 frustum planes
 *
 * + Projection: the surviving vertices are multiplied by the Perspective Projection Matrix
 *
 * + Image Space (NDC): finally, the projected vertices pass through Perspective Divide to be transformed into Screen Space
 *   NDC: Normalized Device Coordinates:
 *          +1
 *      ---------
 *   -1 |       | +1
 *      |       |
 *      ---------
 *          -1
 *
 *   Note: as an alternative to clipping in Camera Space (Frustum Clipping), Homogeneous Clipping could be done in Image Space (NDC).
 *
 * + Screen Space: the verte is translated into the middle of the screen for rendering and things are ready to be rasterized
 *   and have proper X,Y coordinates within the bounds of the monitor to be
 */
void Window::_processGraphicsPipeline(Mesh* mesh)
{
    // create a scale matrix that will be used to multiply the mesh vertices
    Mat4 scaleMatrix = Mat4::scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);

    // create a translation matrix that will be used to multiply the mesh vertices
    Mat4 translationMatrix = Mat4::translate(mesh->translation.x, mesh->translation.y, mesh->translation.z);

    // create a translation matrix that will be used to multiply the mesh vertices
    Mat4 rotationMatrixX = Mat4::rotateX(mesh->rotation.x);
    Mat4 rotationMatrixY = Mat4::rotateY(mesh->rotation.y);
    Mat4 rotationMatrixZ = Mat4::rotateZ(mesh->rotation.z);

    // loop through faces: for each face (triangle), use the vertex index on the face to get the corresponding vertices
    for (unsigned int f = 0; f < mesh->faces.size(); ++f)
    {
//        if (f != 4) // front face for cube.obj
//            continue;

        // for each triangle face, get the 3 vertices that define it
        Face face = mesh->faces[f];
        Vec3d v1 = mesh->vertices[face.a]; // face.a - 1 hols the index of the 1st vertice of the face
        Vec3d v2 = mesh->vertices[face.b]; // face.b - 1 hols the index of the 2nd vertice of the face
        Vec3d v3 = mesh->vertices[face.c]; // face.c - 1 hols the index of the 3rd vertice of the face
        Vec3d faceVertices[3] = { v1, v2, v3 };

//        printf("face #%d v1=%.1f %.1f %.1f \tv2=%.1f %.1f %.1f \tv3=%.1f %.1f %.1f\n", f,
//                faceVertices[0].x, faceVertices[0].y, faceVertices[0].z,
//                faceVertices[1].x, faceVertices[1].y, faceVertices[1].z,
//                faceVertices[2].x, faceVertices[2].y, faceVertices[2].z);

        // array to store the transformed vertices: A, B, C
        Vec4d transformedVertices[3];

        // loop through all the 3 vertices of the face and apply transformations
        for (unsigned int v = 0; v < 3; ++v)
        {
            //std::cout << "faceVertices[v]=" << faceVertices[v] << std::endl;

            Vec4d transformedVertex = Vec3d::toVec4d(faceVertices[v]); // converts Vec3d to Vec4d

            /* To transform the vertices to World Space, the order of the linear transforms matter:
             *  1. Scale
             *  2. Rotate                   [T] * [R] * [S] * v
             *  3. Translate
             */

            // Create the World Matrix combining Scale, Rotation and Translation matrices
            Mat4 worldMatrix = Mat4::eye();
            worldMatrix = scaleMatrix * worldMatrix;
            worldMatrix = rotationMatrixZ * worldMatrix;
            worldMatrix = rotationMatrixY * worldMatrix;
            worldMatrix = rotationMatrixX * worldMatrix;
            worldMatrix = translationMatrix * worldMatrix;

            // finally, transform the vertex to World Space
            transformedVertex = transformedVertex * worldMatrix;

            // convert the scene (vertices) from World Space to View/Camera Space
            transformedVertex = transformedVertex * _viewMatrix;

            // save each transformed vertex
            transformedVertices[v] = transformedVertex;
        }

        /* Check for Backface culling: do not draw back-faces
         *
         *          A
         *        /   \
         *      /      \
         *    B - - - - C
         *
         *  1. Find vectors B-A and C-A
         *  2. Take their cross product and find the perpendicular normal
         *  3. Find the camera ray vector by subtracting the camera position from point A
         *  4. Take the dot product between the normal N and the camera ray
         *  5. If this dot product is less than zero, then DO NOT display the face
         */

        // calculate the Normal vector of the Face
        Vec3d faceNormal = Vec4d::normal(transformedVertices[0], transformedVertices[1], transformedVertices[2]);

        // check if this face is looking away from the camera and then abort its rendering
        if (ENABLE_FACE_CULL)
        {
            // 3. Find the camera ray vector by subtracting the camera position from point A
            Vec3d origin;
            Vec3d cameraRay = origin - Vec4d::toVec3d(transformedVertices[0]);

            // 4. Take the dot product between the normal N and the camera ray
            float dotNormalCamera = faceNormal.dot(cameraRay); // alignment

            // 5. If this dot product is less than zero, then DO NOT display the face
            if (dotNormalCamera < 0)
                continue;
        }

        /* Check for Frustum Clipping: clip the face when part of it is outside the viewing frustum
         *
         * Make sure the face is inside the viewing Frustum and clip its mesh if necessary
         * to avoid crashes. Clipping a polygon might result in even more vertices.
         */

        Polygon poly(Vec4d::toVec3d(transformedVertices[0]),
                     Vec4d::toVec3d(transformedVertices[1]),
                     Vec4d::toVec3d(transformedVertices[2]),
                     face.a_uv,
                     face.b_uv,
                     face.c_uv);

//        printf("polygon v1=%.1f %.1f %.1f \tv2=%.1f %.1f %.1f \tv3=%.1f %.1f %.1f\n",
//                transformedVertices[0].x, transformedVertices[0].y, transformedVertices[0].z,
//                transformedVertices[1].x, transformedVertices[1].y, transformedVertices[1].z,
//                transformedVertices[2].x, transformedVertices[2].y, transformedVertices[2].z);

        poly.clip(_frustumPlanes);

        // after clipping, break the Polygon down into Triangles
        std::vector<Triangle> triangles = poly.triangles();
        //std::cout << "triangles.size()=" << triangles.size() << std::endl;

        /* Projection: project each of the 3D vertex of a Triangle into their 2D screen representation using Perspective Projection */

        // loop all triangles after clipping
        for (unsigned int t = 0; t < triangles.size(); ++t)
        {
            Triangle triangle = triangles[t];

            Vec4d projectedPoints[3];

            for (unsigned int v = 0; v < 3; ++v)
            {
                /* Projection stage */

                // 1st step: multiply the projection matrix by the original 3D vertex. Converts from View/Camera Space to Screen Space
                projectedPoints[v] = triangle.points[v] * _projMatrix;

                // 2nd step: perspective divide with original Z-value now stored in W (things that are furthest away look smaller)
                // the coordinates after perspective divide are called NDC (normalized device coordinates)
                if (projectedPoints[v].w != 0.0)
                {
                    projectedPoints[v].x /= projectedPoints[v].w;
                    projectedPoints[v].y /= projectedPoints[v].w;
                    projectedPoints[v].z /= projectedPoints[v].w;
                }

                /* Things are now in Screen Space */

                // flip vertically: the Y values from the OBJ file grow in the bottom-up direction, the higher you go,
                // the more positive they are. However, the screen is draw top-bottom since origin (0,0) is on the top-left.
                projectedPoints[v].y *= -1;

                // scale into the view
                projectedPoints[v].x *= _gfx.width() / 2.f;
                projectedPoints[v].y *= _gfx.height() / 2.f;

                // translate them to the center of the screen
                projectedPoints[v].x += _gfx.width() / 2.f;
                projectedPoints[v].y += _gfx.height() / 2.f;
            }


            /* Flat Shading: a per face process that calculates the final triangle color using the face.color or
             * the texture (if there's one), and compute the direction of the light source.
             * Brighter or Darker, depends on how align that Face Normal is with the inverse of the light ray.
             */
            float lightIntensityFactor = -faceNormal.dot(_lightSource.direction);
            uint32_t triangleColor = Light::calcIntensity(face.color, lightIntensityFactor);

            // assemble a projected 4D triangle for a 2D screen: Triangle(Vec4d, Vec4d, Vec4d, color, depth);
            Triangle projectedTriangle = { projectedPoints[0], projectedPoints[1], projectedPoints[2],
                                           triangle.texCoords[0], triangle.texCoords[1], triangle.texCoords[2],
                                           mesh->texture, mesh->textureWidth, mesh->textureHeight,
                                           triangleColor };

            // save the projected triangle in the array of triangles that need to be rendered
            _triangles2render.push_back(projectedTriangle);
        }

    } // mesh->faces.size()
}

/* updt: updates animations and object position on the screen
 */
void Window::updt()
{
    // waste a few milliseconds until enough time has passed to update the new frame
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
    qint64 time2wait = RENDER_WAIT_MS - (curTime - _prevTime);

    if (time2wait > 0 && time2wait < RENDER_WAIT_MS)
        QThread::msleep(time2wait); // delay

    // calculate delta time: how much time passed since the last frame?
    curTime = QDateTime::currentMSecsSinceEpoch();
    _deltaTime = (curTime - _prevTime) / 1000.f; // convert to seconds
    //std::cout << "curTime - _prevTime=" << curTime - _prevTime << " (ms)" << std::endl;
    //std::cout << "          deltaTime=" << deltaTime << " (s)" << std::endl;

    // update prevTime for the next frame
    _prevTime = curTime;

    // clear the list of previous projected points
    _triangles2render.clear();

    /* create the view matrix to look at a target point */

    static Vec3d target = _camera.lookAtTarget();
    Vec3d upVector(0, 1, 0);

    // update camera position to orbit around the scene
    if (ORBIT_CAMERA)
    {
        // define where the camera should orbit around
        target = Vec3d(0.f, 0.f, 7.f);

        // increase 15 degrees per second
        _cameraOrbitAngle += 15.f * _deltaTime;

        _camera.position.x = target.x + _cameraOrbitDistance * std::cos(_cameraOrbitAngle * (PI / 180.f));
        _camera.position.z = target.z + _cameraOrbitDistance * std::sin(_cameraOrbitAngle * (PI / 180.f));

        if (_cameraOrbitAngle > 359.f)
            _cameraOrbitAngle = 0.f;
    }

    _viewMatrix = Mat4::lookAt(_camera.position,     // eye
                               target,               // where the camera is looking at (i.e. the direction of the camera)
                               upVector);            // up vector

    for (unsigned int m = 0; m < _meshObjects.size(); ++m)
    {
        Mesh* mesh = &_meshObjects[m];

        // adjust Scale/Rotation/Translation for all the meshes
        //mesh->rotation.x += 0.6f * _deltaTime;
        //mesh->rotation.y += 0.3f * _deltaTime;
        //mesh->rotation.z += 0.0f * _deltaTime;
        //mesh->scale.x += 0.002f;
        //mesh->scale.y += 0.001f;
        //mesh->translation.x += 0.01 * _deltaTime;
        //mesh->translation.z = 5.0;  // translate point away from the camera

        // pass the mesh through the graphics pipeline stages
        _processGraphicsPipeline(mesh);
    }
}

void Window::render(QPainter& p)
{
    //qDebug() << "Window::render";

    // clear the buffer with a solid color
    _gfx.clearColorBuffer(0xFF000000); // black=0xFF000000, white=0xFFFFFFFF

    // clear the depth buffer
    _gfx.clearDepthBuffer(1.0f);

    // draw background grid
    _gfx.drawGrid();

    /* loop projected triangles and render them
     *
     * The loop below simply iterates through every triangle drawing them on the screen without respecting their Z order:
     *      for (unsigned int i = 0; i < _triangles2render.size(); ++i) {
     *          Triangle triangle = _triangles2render[i];
     *          _gfx.drawTriangle(triangle.points[0].x, triangle.points[0].y,
     *                            triangle.points[1].x, triangle.points[1].y,
     *                            triangle.points[2].x, triangle.points[2].y,
     *                            triangle.color);
     *      }
     *
     * A simple solution for the psychodelic problem that it creates is the Painter's Algorithm
     * which painst the triangles that are furthest away first:
     *  - Average the Z coord of all the 3 vertices of a Triangle and assume that is the depth of a face
     */
    for (unsigned int i = 0; i < _triangles2render.size(); ++i) // with face culling enabled, size=2 for a cube that has no rotation
    {
        Triangle triangle = _triangles2render[i];

        // debug: since the triangles are sorted by their Z value, render just the first 2 for the front face
        //if  (i != 0 && i != 1)
        //    continue;

        switch (_renderMode)
        {
            case RENDER_MODE::WIREFRAME:
                // connect the vertices (wireframe, unfilled)
                _gfx.drawTriangle(triangle.points[0].x, triangle.points[0].y,
                                  triangle.points[1].x, triangle.points[1].y,
                                  triangle.points[2].x, triangle.points[2].y,
                                  WIREFRAME_COLOR);
                break;

            case RENDER_MODE::WIREFRAME_DOTS:
                // connect the vertices (wireframe, unfilled)
                _gfx.drawTriangle(triangle.points[0].x, triangle.points[0].y,
                                  triangle.points[1].x, triangle.points[1].y,
                                  triangle.points[2].x, triangle.points[2].y,
                                  WIREFRAME_COLOR);

                // draw small dots points for each vertex (yellow)
                _gfx.drawRect(triangle.points[0].x, triangle.points[0].y, 6, 6, 0xFF00FFFF);
                _gfx.drawRect(triangle.points[1].x, triangle.points[1].y, 6, 6, 0xFF00FFFF);
                _gfx.drawRect(triangle.points[2].x, triangle.points[2].y, 6, 6, 0xFF00FFFF);
                break;

            case RENDER_MODE::TRIANGLES:
                // draw the vertices (filled)
                _gfx.fillTriangle(triangle.points[0], triangle.points[1], triangle.points[2], triangle.color);
                break;

            case RENDER_MODE::TRIANGLES_WIREFRAME:
                // draw the vertices (filled)
                _gfx.fillTriangle(triangle.points[0], triangle.points[1], triangle.points[2], triangle.color);

                // connect the vertices (wireframe, unfilled)
                _gfx.drawTriangle(triangle.points[0].x, triangle.points[0].y,
                                  triangle.points[1].x, triangle.points[1].y,
                                  triangle.points[2].x, triangle.points[2].y,
                                  WIREFRAME_COLOR);
                break;

            case RENDER_MODE::TEXTURED:
                _gfx.drawTexturedTriangle(triangle.points[0], triangle.points[1], triangle.points[2],
                                          triangle.texCoords[0], triangle.texCoords[1], triangle.texCoords[2],
                                          triangle.texture.get(), triangle.textureWidth, triangle.textureHeight,
                                          FIX_TEXTURE_DISTORTION);
                break;

            case RENDER_MODE::TEXTURED_WIREFRAME:
                _gfx.drawTexturedTriangle(triangle.points[0], triangle.points[1], triangle.points[2],
                                          triangle.texCoords[0], triangle.texCoords[1], triangle.texCoords[2],
                                          triangle.texture.get(), triangle.textureWidth, triangle.textureHeight,
                                          FIX_TEXTURE_DISTORTION);

                // connect the vertices (wireframe, unfilled)
                _gfx.drawTriangle(triangle.points[0].x, triangle.points[0].y,
                                  triangle.points[1].x, triangle.points[1].y,
                                  triangle.points[2].x, triangle.points[2].y,
                                  WIREFRAME_COLOR);
                break;

            default:
                qDebug() << "Window::render !!! Unknown render mode";
                break;
        }

    }

    // copy Color Buffer to "texture" so that it can be draw on the screen
    _renderColorBuffer(p);
}

void Window::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Escape:
            qDebug() << "keyPressEvent: ESC";
            exit(0);
            break;

        /* rendering mode */

        case Qt::Key_1:
            qDebug() << "keyPressEvent: RENDER_MODE::WIREFRAME";
            _renderMode = RENDER_MODE::WIREFRAME;
            break;

        case Qt::Key_2:
            qDebug() << "keyPressEvent: RENDER_MODE::WIREFRAME_DOTS";
            _renderMode = RENDER_MODE::WIREFRAME_DOTS;
            break;

        case Qt::Key_3:
            qDebug() << "keyPressEvent: RENDER_MODE::TRIANGLES";
            _renderMode = RENDER_MODE::TRIANGLES;
            break;

        case Qt::Key_4:
            qDebug() << "keyPressEvent: RENDER_MODE::TRIANGLES_WIREFRAME";
            _renderMode = RENDER_MODE::TRIANGLES_WIREFRAME;
            break;

        case Qt::Key_5:
            ENABLE_FACE_CULL = !ENABLE_FACE_CULL;
            qDebug() << "keyPressEvent: ENABLE_FACE_CULL=" << ENABLE_FACE_CULL;
            break;

        case Qt::Key_6:
            qDebug() << "keyPressEvent: RENDER_MODE::TEXTURED";
            _renderMode = RENDER_MODE::TEXTURED;
            break;

        case Qt::Key_7:
            qDebug() << "keyPressEvent: RENDER_MODE::TEXTURED_WIREFRAME";
            _renderMode = RENDER_MODE::TEXTURED_WIREFRAME;
            break;

        case Qt::Key_T:
            FIX_TEXTURE_DISTORTION = !FIX_TEXTURE_DISTORTION;
            qDebug() << "keyPressEvent: FIX_TEXTURE_DISTORTION=" << FIX_TEXTURE_DISTORTION;
            break;

        /* camera movement */

        case Qt::Key_O:
            ORBIT_CAMERA = !ORBIT_CAMERA;
            qDebug() << "keyPressEvent: ORBIT_CAMERA=" << ORBIT_CAMERA;
            break;

        case Qt::Key_W:
            qDebug() << "keyPressEvent: W";
            _camera.rotatePitch(+1.0f * _deltaTime);
            break;

        case Qt::Key_S:
            qDebug() << "keyPressEvent: S";
            _camera.rotatePitch(-1.0f * _deltaTime);
            break;

        case Qt::Key_Up:
            qDebug() << "keyPressEvent: Up";
            _camera.forwardVelocity = _camera.direction * 5.f * _deltaTime;
            _camera.position = _camera.position + _camera.forwardVelocity;
            break;

        case Qt::Key_Down:
            qDebug() << "keyPressEvent: Down";
            _camera.forwardVelocity = _camera.direction * 5.f * _deltaTime;
            _camera.position = _camera.position - _camera.forwardVelocity;
            break;

        case Qt::Key_Right:
            qDebug() << "keyPressEvent: RIGHT";
            //_camera.yaw += 1.0f * _deltaTime;
            _camera.rotateYaw(+1.0f * _deltaTime);
            break;

        case Qt::Key_Left:
            qDebug() << "keyPressEvent: LEFT";
            //_camera.yaw -= 1.0f * _deltaTime;
            _camera.rotateYaw(-1.0f * _deltaTime);
            break;

        default:
            break;
    }
}
