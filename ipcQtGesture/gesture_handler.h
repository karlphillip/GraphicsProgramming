/* Copyright (C) 2013 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#pragma once
#include "util_render.h"
#include "util_pipeline.h"

#include <QElapsedTimer>
#include <QSize>
#include <QObject>
#include <QTimer>
#include <QPixmap>


class GestureHandler : public QObject
{
    Q_OBJECT
public:
    GestureHandler();
    ~GestureHandler();

    bool start();
    void stop();
    QSize image_size();

signals:
    void notify_gesture(PXCGesture::Gesture gesture_evt);
    void notify_image(QPixmap pixmap);

private slots:
    void _stream();

private:
    bool _streaming;
    bool _init_error;
    QTimer _fps_timer;
    UtilPipeline _pipeline;
    QElapsedTimer _elapsed_timer;
    PXCGesture::Gesture::Label _last_gesture;
};
