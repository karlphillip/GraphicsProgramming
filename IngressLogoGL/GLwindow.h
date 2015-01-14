/* Copyright (C) 2012 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 *
 *
 * Ingress logo with glowing effect, based on Nehe lesson 36.
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
