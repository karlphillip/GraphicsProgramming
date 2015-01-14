/* Copyright (C) 2013 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "widget.h"
#include <GL/GLU.h>

#include <iostream>
#include <QKeyEvent>
#include <QTimer>


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
    update();

    // Set timer according to FPS
    QTimer::singleShot(1000/_fps, this, SLOT(_tick()));
}

void GLWidget::initializeGL()
{
    // Set clear color as black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
    _fps = cv_capture.get(CV_CAP_PROP_FPS);
    if (!_fps) // if the function fails, fps is set to 15
        _fps = 15;

    std::cout << "GLWidget::initializeGL: " << _fps << " fps" << std::endl;

    /* Start the timer */

    _tick();
}

void GLWidget::paintGL()
{
    // Clear the screen and depth buffer (with black)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Select the model view matrix and reset it
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, 0.0f);

    // Abort drawing if OpenCV was unable to open the camera
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to open camera" << std::endl;
        return;
    }

    // Note: trying to retrieve more frames than the camera can give you
    // will make the output video blink a lot.
    cv_capture >> cv_frame;
    if (cv_frame.empty())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to retrieve frame" << std::endl;
        return;
    }
    cv::cvtColor(cv_frame, cv_frame, CV_BGR2RGBA);

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
        glTexCoord2f(0, 0);                         glVertex2f(1.0, 1.0);
        glTexCoord2f(cv_frame.cols, 0);             glVertex2f(-1.0, 1.0);
        glTexCoord2f(cv_frame.cols, cv_frame.rows); glVertex2f(-1.0, -1.0);
        glTexCoord2f(0, cv_frame.rows);             glVertex2f(1.0, -1.0);
    glEnd();

    glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void GLWidget::resizeGL( int w, int h)
{
    _width = w;
    _height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);  // Select the projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (h == 0)  // Calculate aspect ratio of the window
       gluPerspective(60, (float) w, 1.0, 50.0);
    else
       gluPerspective(60, (float) w / (float) h, 1.0, 50.0);

    gluLookAt(0.0,  0.0, 2.0,   // eye
              0.0,  0.0, 0.0,   // center
              0.0,  1.0, 0.0);  // up

    glMatrixMode(GL_MODELVIEW);  // Select the model view matrix
    glLoadIdentity();           // Reset the model view matrix
}
