#pragma once
#include "gesture_handler.h"

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QStatusBar>


class IPCWindow: public QMainWindow
{
    Q_OBJECT
public:
    IPCWindow();
    ~IPCWindow();

    bool start_streaming();
    void stop_streaming();

    void keyPressEvent(QKeyEvent* evt);

private slots:
    void _gesture_evt(PXCGesture::Gesture gesture_evt);
    void _image_evt(QPixmap pixmap);

private:
    QLabel* _label;
    QLabel* _status_label;
    GestureHandler _gesture_handler;
};
