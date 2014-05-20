#pragma once
#include "time_canvas.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>


class TimeAnimation : public QWidget
{
    Q_OBJECT
public:
    TimeAnimation(QWidget* parent = NULL);

public slots:
    void reset();

private slots:
    void _tick();

private:
    QGroupBox* _group_box;
    QVBoxLayout* _main_layout;
    QGridLayout* _h_layout;
    TimeCanvas* _black_canvas, *_blue_canvas, *_red_canvas;
    QLabel* _black_label, *_blue_label, *_red_label;
    qint64 _last_time_60fps, _last_time_30fps, _last_time_10fps;
    int _max_fps;
    int _counter;
};
