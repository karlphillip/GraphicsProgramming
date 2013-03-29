#pragma once
#include <cv.h>
#include <highgui.h>
#include <QGLWidget>
#include <QImage>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = 0);
    virtual ~GLWidget();

    /* OpenGL initialization, viewport resizing, and painting */

    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height);

private:
    int _width;
    int _height;
    cv::Mat cv_frame;
    cv::VideoCapture cv_capture;
    int _fps;
    GLuint  _texture;

protected slots:
    void _tick();
};
