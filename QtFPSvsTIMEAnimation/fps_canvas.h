#pragma once
#include <QFrame>


class FpsCanvas : public QFrame
{
    Q_OBJECT
public:
    FpsCanvas(QFrame* parent = NULL, const QColor color = Qt::black);

    void draw();
    void reset();

    void paintEvent(QPaintEvent* event);

private:
    QColor _color;
    int _sq_sz;    
    int _x, _y;
    int _dx, _dy;
};
