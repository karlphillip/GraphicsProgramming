/* Copyright (C) 2013 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "ipc_window.h"

#include <iostream>

#include <QApplication>


IPCWindow::IPCWindow()
{
    setStyleSheet("background-color: black;");
    setFixedSize(640, 480);

    _label = new QLabel(this);
    _label->setGeometry(QRect(0, 0, 640, 480));

    QSize img_sz = _gesture_handler.image_size();
    if (img_sz.width() && img_sz.height())
    {
        setFixedSize(img_sz.width(), img_sz.height());
        _label->setGeometry(QRect(0, 0, img_sz.width(), img_sz.height()));
    }

    _status_label = new QLabel(statusBar());
    _status_label->setAlignment(Qt::AlignCenter);
    _status_label->setStyleSheet("color: orange; background-color: black;");
    QFont sb_font("Arial", 11);
    sb_font.setBold(true);
    _status_label->setFont(sb_font);
    _status_label->setText("Running...");
    statusBar()->addWidget(_status_label, 1);

    QObject::connect(&_gesture_handler, SIGNAL(notify_gesture(PXCGesture::Gesture)),
                                  this, SLOT(_gesture_evt(PXCGesture::Gesture)));

    QObject::connect(&_gesture_handler, SIGNAL(notify_image(QPixmap)),
                                  this, SLOT(_image_evt(QPixmap)));

    if (!_gesture_handler.start())
    {
        _status_label->setText("!!! Failed to start gesture handler.");
        qApp->quit();
    }
}

IPCWindow::~IPCWindow()
{
}

void IPCWindow::_gesture_evt(PXCGesture::Gesture g_evt)
{
    switch (g_evt.label)
    {
        case PXCGesture::Gesture::LABEL_HAND_WAVE:
            _status_label->setText("WAVE");
        break;

        case PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT:
            _status_label->setText("SWIPE LEFT");
        break;

        case PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT:
            _status_label->setText("SWIPE RIGHT");
        break;

        case PXCGesture::Gesture::LABEL_NAV_SWIPE_UP:
            _status_label->setText("SWIPE UP");
        break;

        case PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN:
            _status_label->setText("SWIPE DOWN");
        break;

        case PXCGesture::Gesture::LABEL_HAND_CIRCLE:
            _status_label->setText("CIRCLE");
        break;

        case PXCGesture::Gesture::LABEL_POSE_THUMB_UP:
            _status_label->setText("THUMB UP");
            //std::cout << "THUMB UP" << std::endl;
        break;

        case PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN:
            _status_label->setText("THUMB DOWN");
            //std::cout << "THUMB DOWN" << std::endl;
        break;

        case PXCGesture::Gesture::LABEL_POSE_PEACE:
            _status_label->setText("PEACE");
            //std::cout << "PEACE" << std::endl;
        break;

        case PXCGesture::Gesture::LABEL_POSE_BIG5:
            _status_label->setText("BIG5");
            //std::cout << "BIG5" << std::endl;
        break;

        default:
            QString str("UNKNOWN: ");
            str += g_evt.label;
            _status_label->setText(str);
        break;
    }
}

void IPCWindow::_image_evt(QPixmap pixmap)
{
    _label->setPixmap(pixmap);
}

void IPCWindow::keyPressEvent(QKeyEvent* evt)
{
    switch (evt->key())
    {
        case Qt::Key_Escape:
            qApp->quit();
            break;

        default:
            evt->ignore();
            break;
    }
}
