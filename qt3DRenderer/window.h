#pragma once
#include <vector>

#include <QWidget>
#include <QImage>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>

#include "mat4.h"
#include "vec3d.h"
#include "display.h"
#include "light.h"
#include "mesh.h"
#include "cubemesh.h"
#include "triangle.h"
#include "camera.h"
#include "clipping.h"


enum RENDER_MODE {
    WIREFRAME,              // draw only wireframe lines
    WIREFRAME_DOTS,         // draw wireframe lines with small colored dots on each triangle vertex
    TRIANGLES,              // fills triangles with a solid color
    TRIANGLES_WIREFRAME,    // fills triangles and adds wireframe lines
    TEXTURED,               // draw with textures
    TEXTURED_WIREFRAME      // draw with textures and adds wireframe lines
};


class Window : public QWidget
{
    Q_OBJECT

public:
    Window();
    ~Window();


    void updt();
    void render(QPainter& p);

    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);

private slots:
    void _tick();

private:
    void _renderColorBuffer(QPainter& p);
    void _initFrustumPlanes(const float& fovX, const float& fovY, const float& zNear, const float& zFar);
    void _processGraphicsPipeline(Mesh* mesh);

    int _width, _height;
    QImage _framebuffer;
    Display _gfx;

    std::vector<Triangle> _triangles2render;
    std::vector<Mesh> _meshObjects;

    Camera _camera;
    float _cameraOrbitAngle;
    float _cameraOrbitDistance;

    Mat4 _viewMatrix;
    float _deltaTime;

    QTimer* _timer;
    qint64 _tick_ms;
    qint64 _prevTime;
    RENDER_MODE _renderMode;

    Mat4 _projMatrix;
    Light _lightSource;            

    Plane _frustumPlanes[6];
};
