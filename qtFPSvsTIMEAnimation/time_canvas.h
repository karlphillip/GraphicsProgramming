/* Copyright (C) 2014 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
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
