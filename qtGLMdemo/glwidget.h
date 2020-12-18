#pragma once
#include <QGLWidget>

/* extern "C" avoids linker errors related to: unresolved external symbol glmUnitize, glmFacetNormals, ...
 * GLM by Nate Robins is available at: https://user.xmission.com/~nate/tutors.html
 */
extern "C" {
#include "glm.h"
}


class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = 0);
    virtual ~GLWidget();

    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height);

    void _draw_terrain();
    void _draw_model();

    void keyPressEvent(QKeyEvent *e);

private:
    int _width, _height;

    GLMmodel* _obj_model;
    GLuint _carList;

    int _orbitAngle;
    float _cameraPos[3];

protected slots:
    void _tick();
};
