#include "time_canvas.h"
#include <cmath>

#include <QPainter>
#include <QDebug>


TimeCanvas::TimeCanvas(QFrame* parent, const QColor color)
: QFrame(parent), _color(color)
{
    // Initialize class members    
    _x = _y = 50;
    _dx = 2;
    _dy = 1;
    _sq_sz = 10;

    // Set canvas size
    setMinimumSize(175, 175);
}

void TimeCanvas::move(float dt)
{
    /* Update position of square */

    _x += std::ceil((_dx * dt * 60) / 1000);    // (2 * 16 * 60) / 1000 --> 1.92

    if (_dy > 0)
    {
        // If _dy is positive, round up, else animation will move sideways only
        _y += std::ceil((_dy * dt * 60) / 1000);    // (1 * 16 * 60) / 1000 = 0.96 = 1
    }
    else
    {
        // If _dy is negative, round down, else animation will not bounce up
        _y += std::floor((_dy * dt * 60) / 1000);    // (-1 * 16 * 60) / 1000 = -0.96 = -1
    }

    if ( _x <= 0 || (_x >= (width()-1) - _sq_sz) ) {
        _dx *= -1;
    }

    if ( _y <= 0 || (_y >= (height()-1) - _sq_sz) ) {
        _dy *= -1;
    }
}

void TimeCanvas::draw()
{
    // Trigger paintEvent()
    repaint();
}

void TimeCanvas::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    /* Paint white background */

    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::white);

    /* Paint black frame around the canvas */

    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width()-1, height()-1);

    /* Draw square */

    painter.fillRect(_x, _y, _sq_sz, _sq_sz, _color);
}

void TimeCanvas::reset()
{
    _x = _y = 50;
    _dx = 2;
    _dy = 1;
}
