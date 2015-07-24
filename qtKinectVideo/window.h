/* Copyright (C) 2015 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#pragma once

// Kinect for Windows Architecture https://msdn.microsoft.com/en-us/library/jj131023.aspx
#include <windows.h>
#include <NuiApi.h>

#include <QImage>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPainter>
#include <QTimer>


class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();
    ~Window();

    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);


private slots:
    void _tick();

private:
    bool _initKinect();
    void _draw_video_frame(QPainter& painter);

    int _tick_ms;
    Qt::AspectRatioMode _ar_mode;
    int _fps;
    int _video_width;
    int _video_height;
    QImage _image;
    QTimer* _timer;

    INuiSensor* _nui_sensor;
    HANDLE _rgb_stream; //32bit
};
