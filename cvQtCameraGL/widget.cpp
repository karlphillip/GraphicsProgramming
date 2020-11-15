/* Copyright (C) 2013-2020 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "widget.h"
#include <iostream>
#include <QKeyEvent>
#include <QTimer>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Enables the use of 3D perspective
//#define USE_PERSPECTIVE


GLWidget::GLWidget(QWidget *parent)
: QGLWidget(parent)
{
    _width = 0;
    _height = 0;
    _texture = 0;
    _fps = 0;
}

GLWidget::~GLWidget()
{
    glDeleteTextures(1, &_texture);
}

void GLWidget::_tick()
{
    // triggers paintGL()
    updateGL();

    // Set timer according to FPS
    QTimer::singleShot(1000/_fps, this, SLOT(_tick()));
}

void GLWidget::initializeGL()
{
    // Set clear color as black
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // Select pixel storage mode used by glTexImage2D
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // Create the texture
    glGenTextures(1, &_texture);

    /* Open default camera device */

    cv_capture.open(0);
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::initializeGL: !!! Failed to open camera" << std::endl;
        return;
    }

    // Retrieve FPS from the camera
    _fps = cv_capture.get(cv::CAP_PROP_FPS);
    if (!_fps) // if the function fails, fps is set to 15
        _fps = 15;

    std::cout << "GLWidget::initializeGL: " << _fps << " fps" << std::endl;

    // Retrieve the width and height of the captured frame
    _width = cv_capture.get(cv::CAP_PROP_FRAME_WIDTH);
    _height = cv_capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    resize(QSize(_width, _height));

    std::cout << "GLWidget::initializeGL: capture size=" << _width << "x" << _height << std::endl;
    std::cout << "GLWidget::initializeGL: window size=" << this->width() << "x" << this->height() << std::endl;

    /* Start the timer */

    _tick();
}

void GLWidget::paintGL()
{
    // Abort drawing if OpenCV was unable to open the camera
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to open camera" << std::endl;
        return;
    }

    // Clear the screen and depth buffer (with black) then S«select the model view matrix and reset it
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#ifdef USE_PERSPECTIVE
    glTranslatef(0.0f, 0.0f, -1.5);
#endif

    // Note: trying to retrieve more frames than the camera can give you makes the output video blink a lot
    cv_capture >> cv_frame;
    if (cv_frame.empty())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to retrieve frame" << std::endl;
        return;
    }

    //std::cout << "GLWidget::paintGL: frame size " << cv_frame.cols << "x" << cv_frame.rows << std::endl;

    cv::cvtColor(cv_frame, cv_frame, cv::COLOR_BGR2RGBA);

    glEnable(GL_TEXTURE_RECTANGLE_ARB);

    // Typical texture generation using data from the bitmap
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _texture);

    // Transfer image data to the GPU
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,
                 GL_RGBA, cv_frame.cols, cv_frame.rows, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, cv_frame.data);
    if (glGetError() != GL_NO_ERROR)
    {
        std::cout << "GLWidget::paintGL: !!! Failed glTexImage2D" << std::endl;
    }

    // Draw a 2D face with texture
    glBegin(GL_QUADS);
#ifdef USE_PERSPECTIVE
        // glPerspective (3D)
        glTexCoord2f(0, 0);                         glVertex2f(1.0, 1.0);
        glTexCoord2f(cv_frame.cols, 0);             glVertex2f(-1.0, 1.0);
        glTexCoord2f(cv_frame.cols, cv_frame.rows); glVertex2f(-1.0, -1.0);
        glTexCoord2f(0, cv_frame.rows);             glVertex2f(1.0, -1.0);
#else
        // glOrtho (2D)
        glTexCoord2f(0, 0);                         glVertex2f(   0.0,     0.0);
        glTexCoord2f(cv_frame.cols, 0);             glVertex2f(_width,     0.0);
        glTexCoord2f(cv_frame.cols, cv_frame.rows); glVertex2f(_width, _height);
        glTexCoord2f(0, cv_frame.rows);             glVertex2f(   0.0, _height);
#endif
    glEnd();

    glDisable(GL_TEXTURE_RECTANGLE_ARB);

    glFlush();
}

void glPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble ymax = zNear * tan( fovy * M_PI / 360.0 );
    GLdouble ymin = -ymax;
    GLdouble xmin = ymin * aspect;
    GLdouble xmax = ymax * aspect;

    glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void glOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
    glOrtho(left, right, bottom, top, -1, 1);
}

void GLWidget::resizeGL(int w, int h)
{
    _width = w;
    _height = h;

    glViewport     ( 0, 0, _width, _height );
    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ( );

#ifdef USE_PERSPECTIVE
    if (h == 0)
        glPerspective( 60, ( float ) w, 1.0, 100.0 );
    else
        glPerspective( 60, ( float ) w / ( float ) h, 1.0, 100.0 );
#else
    glOrtho(0, _width, _height, 0, 1, -1);
#endif

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
