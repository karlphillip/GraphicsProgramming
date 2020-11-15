/* Copyright (C) 2014 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "time_anim_improved.h"

#include <qDebug>


TimeAnimationImproved::TimeAnimationImproved(QWidget* parent)
: QWidget(parent)
{    
    _counter = 0;
    _accumulator60 = _accumulator30 = _accumulator10 = 0;
    _max_fps = 60;
    _constant_dt = 1000 / _max_fps;
    _last_time_60fps = _last_time_30fps = _last_time_10fps = QDateTime::currentMSecsSinceEpoch();

    _black_canvas = new TimeCanvas((QFrame*)this, Qt::black);
    _blue_canvas = new TimeCanvas((QFrame*)this, Qt::blue);
    _red_canvas = new TimeCanvas((QFrame*)this, Qt::red);

    _black_label = new QLabel(this);
    _black_label->setText("60 FPS");
    _blue_label = new QLabel(this);
    _blue_label->setText("30 FPS");
    _red_label = new QLabel(this);
    _red_label->setText("10 FPS");

    _h_layout = new QGridLayout(this);
    _h_layout->addWidget(_black_canvas, 0, 0, 1, 1);
    _h_layout->addWidget(_blue_canvas, 0, 1, 1, 1);
    _h_layout->addWidget(_red_canvas, 0, 2, 1, 1);
    _h_layout->addWidget(_black_label, 3, 0, 1, 1);
    _h_layout->addWidget(_blue_label, 3, 1, 1, 1);
    _h_layout->addWidget(_red_label, 3, 2, 1, 1);

    _group_box = new QGroupBox(this);
    _group_box->setTitle("Time-based Animation Improved");
    _group_box->setLayout(_h_layout);

    _main_layout = new QVBoxLayout(this);
    _main_layout->addWidget(_group_box);
    setLayout(_main_layout);

    // Start timer
    QTimer::singleShot(1000/_max_fps, this, SLOT(_tick()));
}

void TimeAnimationImproved::_tick()
{
    /* Decide which canvas should be updated */

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    // 30 FPS canvas must be draw once every 2x
    if (_counter % 2 == 0)
    {
        // Compute how much time passed since the last update
        qint64 passed = now - _last_time_30fps;

        _accumulator30 += passed;
        while (_accumulator30 >= _constant_dt)
        {
            _blue_canvas->move(_constant_dt);
            _accumulator30 -= _constant_dt;
        }

        _blue_canvas->draw();
        _last_time_30fps = now;
    }

    // 10 FPS canvas must be draw once every 6x
    if (_counter % 6 == 0)
    {
        // Compute how much time passed since the last update
        qint64 passed = now - _last_time_10fps;

        _accumulator10 += passed;
        while (_accumulator10 >= _constant_dt)
        {
            _red_canvas->move(_constant_dt);
            _accumulator10 -= _constant_dt;
        }

        _red_canvas->draw();
        _last_time_10fps = now;
    }

    // 60 FPS canvas must be draw every time, so we ignore _counter
    {
        // Compute how much time passed since the last update
        qint64 passed = now - _last_time_60fps;

        _accumulator60 += passed;
        while (_accumulator60 >= _constant_dt)
        {
            _black_canvas->move(_constant_dt);
            _accumulator60 -= _constant_dt;
        }

        _black_canvas->draw();
        _last_time_60fps = now;
    }

    _counter++;
    _counter = _counter % 60; // Ranges from 0 to 59

    // Reset the timer
    QTimer::singleShot(1000/_max_fps, this, SLOT(_tick()));
}

void TimeAnimationImproved::reset()
{
    _black_canvas->reset();
    _blue_canvas->reset();
    _red_canvas->reset();
}
