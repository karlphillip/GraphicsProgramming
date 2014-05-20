#pragma once
#include <QFrame>
#include <QTimer>
#include <QDateTime>


class TimeCanvas : public QFrame
{
    Q_OBJECT
public:
    TimeCanvas(QFrame* parent = NULL, const QColor color = Qt::black);

    void move(float dt);
    void draw();
    void reset();

    void paintEvent(QPaintEvent* event);

private:
    QColor _color;
    int _sq_sz;
    int _x, _y;
    int _dx, _dy;
};
