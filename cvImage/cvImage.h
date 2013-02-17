#pragma once
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include <highgui.h>


class cvImage : public QMainWindow
{
    Q_OBJECT

public:
    cvImage();
    ~cvImage();

    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QMenuBar _menu_bar;
    QMenu* _menu;
    QImage* _image;

private slots:
    void _open();

protected:
    void _close();

signals:
    void closed();
};
