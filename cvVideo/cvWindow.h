#pragma once
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include <highgui.h>


class cvWindow : public QWidget
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
