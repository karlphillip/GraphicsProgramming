#include "fps_anim.h"

#include <QTimer>


FrameAnimation::FrameAnimation(QWidget* parent)
: QWidget(parent)
{
    _max_fps = 60;
    _counter = 0;

    _black_canvas = new FpsCanvas((QFrame*)this);
    _blue_canvas = new FpsCanvas((QFrame*)this, Qt::blue);
    _red_canvas = new FpsCanvas((QFrame*)this, Qt::red);

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
    _group_box->setTitle("Frame-based Animation");
    _group_box->setLayout(_h_layout);

    _main_layout = new QVBoxLayout(this);
    _main_layout->addWidget(_group_box);
    setLayout(_main_layout);

    // Start timer
    QTimer::singleShot(1000/_max_fps, this, SLOT(_tick()));
}

void FrameAnimation::_tick()
{
    /* Decide which canvas should be updated */

    // 30 FPS canvas must be draw once every 2x
    if (_counter % 2 == 0)
        _blue_canvas->draw();

    // 10 FPS canvas must be draw once every 6x
    if (_counter % 10 == 0)
        _red_canvas->draw();

    _counter++;
    _counter = _counter % 60; // Ranges from 0 to 59

    // 60 FPS canvas must be draw every time, so we ignore _counter
    _black_canvas->draw();

    // Reset the timer
    QTimer::singleShot(1000/_max_fps, this, SLOT(_tick()));
}

void FrameAnimation::reset()
{
    _black_canvas->reset();
    _blue_canvas->reset();
    _red_canvas->reset();
}
