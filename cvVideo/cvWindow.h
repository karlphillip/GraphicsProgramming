/* Copyright (C) 2013 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#pragma once
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include <highgui.h>


class cvWindow : public QMainWindow
{
    Q_OBJECT

public:
    cvWindow();
    ~cvWindow();

    void _draw_video_frame(QPainter& painter);
    void _draw_info_text(QPainter& painter);

    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);

private:
    QMenuBar _menu_bar;
    QMenu* _menu;
    QImage* _image;
    cv::VideoCapture* _capture;
    int _tick_ms;
    Qt::AspectRatioMode _ar_mode;
    int _fps;
    int _video_width;
    int _video_height;
    QTimer* _timer;

private slots:
    void _open();
    void _tick();

protected:
    void _close();

signals:
    void closed();
};
