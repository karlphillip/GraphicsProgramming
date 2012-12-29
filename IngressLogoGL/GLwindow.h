/*     GLwindow.h
 *
 *     Karl Phillip Buhr, 2012, 2013
 *     karlphillip@gmail.com
 *     http://stackoverflow.com/users/176769/karlphillip
 *
 *     Ingress logo with glowing effect, based on Nehe lesson 36.
 */
#pragma once

#include <QGLWidget>

class GLwindow : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLwindow(QWidget* parent = 0);
    virtual ~GLwindow();

    /* OpenGL initialization, viewport resizing, and painting */

    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height);

    /* Helper functions */

    GLuint _gen_empty_texture();
    void _render_to_texture();
    void _draw_blur(int times, float inc);

    void _view_ortho();
    void _view_perspective();

    void _draw_ingress_logo();
    void _draw_quad();
    void _draw_triangle();

private:
    int _width;
    int _height;

    GLuint _blur_texture;

protected slots:
    void _tick();
};
