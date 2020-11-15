/* Copyright (C) 2014 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "fps_canvas.h"

#include <QPainter>
#include <QDebug>


FpsCanvas::FpsCanvas(QFrame* parent, const QColor color)
: QFrame(parent), _color(color)
{
    // Initialize class members
    _x = _y = 50;
    _dx = 2;
    _dy = 1;
    _sq_sz = 10;

    // Set size of the canvas
    setMinimumSize(175, 175);
}

void FpsCanvas::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);

    // Paint white background
    painter.fillRect(0, 0, width(), height(), Qt::white);

    // Paint black frame around the canvas
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width()-1, height()-1);

    /* Move square */

    _x += _dx;
    _y += _dy;

    if ( _x <= 0 || (_x >= (width()-1) - _sq_sz) )
        _dx = -_dx;

    if ( _y <= 0 || (_y >= (height()-1) - _sq_sz) )
        _dy = -_dy;

    // Draw square
    painter.fillRect(_x, _y, _sq_sz, _sq_sz, _color);
}

void FpsCanvas::draw()
{
    // Trigger paintEvent()
    repaint();
}

void FpsCanvas::reset()
{
    _x = _y = 50;
    _dx = 2;
    _dy = 1;
}
