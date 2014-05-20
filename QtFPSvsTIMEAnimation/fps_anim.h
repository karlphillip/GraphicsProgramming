#pragma once
#include "fps_canvas.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>


class FrameAnimation : public QWidget
{
    Q_OBJECT
public:
    FrameAnimation(QWidget* parent = NULL);

public slots:
    void reset();

private slots:
    void _tick();

private:
    QGroupBox* _group_box;
    QVBoxLayout* _main_layout;
    QGridLayout* _h_layout;
    FpsCanvas* _black_canvas, *_blue_canvas, *_red_canvas;
    QLabel* _black_label, *_blue_label, *_red_label;
    int _max_fps;
    int _counter;
};
