#include "cvWindow.h"

#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QTimer>
#include <QApplication>

#include <cv.h>


cvWindow::cvWindow()
: _menu(NULL), _image(NULL), _capture(NULL), _tick_ms(33), _ar_mode(Qt::IgnoreAspectRatio),
  _fps(0), _video_width(0), _video_height(0), _timer(NULL)
{
    setWindowTitle(tr("QT Video demo with OpenCV"));
    resize(480, 240);

    _menu = new QMenu("File");
    _menu->addAction("Op3n", this, SLOT(_open()));
    _menu->addSeparator();
    _menu->addAction("Ex1t", this, SLOT(close()));
    _menu_bar.addMenu(_menu);

	setMenuBar(&_menu_bar);
}

cvWindow::~cvWindow()
{
    if (_timer)
    {
        _timer->stop();
        delete _timer;
    }

    if (_menu)
        delete _menu;

    if (_image)
        delete _image;

    if (_capture)
    {
        _capture->release();
        delete _capture;
    }
}

void cvWindow::_tick()
{
    /* This method is called every couple of milliseconds.
     * It reads from the OpenCV's capture interface and saves a frame as QImage
     */
    cv::Mat frame;
    if (!_capture->read(frame))
    {
        qDebug() << "cvWindow::_tick !!! Failed to read frame from the capture interface";
    }

    // Since OpenCV uses BGR order, we need to convert it to RGB
    cv::cvtColor(frame, frame, CV_BGR2RGB);

    // Copy cv::Mat to QImage
    memcpy(_image->scanLine(0), (unsigned char*)frame.data, _image->width() * _image->height() * frame.channels());

    // The same as above, but much worst.
    //QImage img = QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    //*_image = img.copy();

    // Trigger paint event to redraw the window
    update();
}

void cvWindow::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    // When no image is loaded, paint the window with black
    if (!_image)
    {
        painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::black);
        QWidget::paintEvent(e);
        return;
    }

    // Draw a frame from the video
    _draw_video_frame(painter);

    // Draw text with info about the video
    _draw_info_text(painter);

    QWidget::paintEvent(e);
}

void cvWindow::_draw_video_frame(QPainter& painter)
{
    // To simply draw on the window, execute the code below.
    //painter.drawImage(QPoint(0, 0), *_image);

    // But we are drawing the image according to AR settings:
    // http://doc.qt.digia.com/stable/qt.html#AspectRatioMode-enum
    switch (_ar_mode)
    {
        default:
        case Qt::IgnoreAspectRatio:
        {
            painter.drawImage(QRectF(QPoint(0, 0), QSize(width(), height())), *_image, QRectF(QPoint(0, 0), _image->size()));
        }
        break;

        case Qt::KeepAspectRatio:
        {
            painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::black);

            QImage scaled_img = _image->scaled(QSize(width(), height()), Qt::KeepAspectRatio, Qt::FastTransformation);

            painter.drawImage(qRound(width()/2)  - qRound(scaled_img.size().width()/2),
                              qRound(height()/2) - qRound(scaled_img.size().height()/2),
                              scaled_img);
        }
        break;

        case Qt::KeepAspectRatioByExpanding:
        {
            QImage scaled_img = _image->scaled(QSize(width(), height()), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

            painter.drawImage(QRectF(QPoint(0, 0), QSize(width(), height())),
                               scaled_img,
                               QRectF(QPoint(qRound(scaled_img.size().width()/2) - qRound(width()/2),
                                             qRound(scaled_img.size().height()/2) - qRound(height()/2)),
                                             QSize(width(), height())));
        }
        break;
    }
}

void cvWindow::_draw_info_text(QPainter& painter)
{
    // Setup font properties
    painter.setFont(QFont("Arial", 16));
    painter.setPen(QPen(QColor("#2e8b57")));

    // Draw info about video dimensions
    QString video_size = QString::number(_video_width) + "x" + QString::number(_video_height) ;
    painter.drawText(QRect(10, 6, 100, 100), Qt::AlignLeft, video_size);

    // Draw info about FPS
    QString fps = "FPS: " + QString::number(_fps);
    painter.drawText(QRect(10, 22, 100, 100), Qt::AlignLeft, fps);

    // Draw selected AR mode
    switch (_ar_mode)
    {
        case Qt::IgnoreAspectRatio:
            painter.drawText(QRect(10, 37, 300, 100), Qt::AlignLeft, "IgnoreAspectRatio");
        break;

        case Qt::KeepAspectRatio:
            painter.drawText(QRect(10, 37, 300, 100), Qt::AlignLeft, "KeepAspectRatio");
        break;

        case Qt::KeepAspectRatioByExpanding:
            painter.drawText(QRect(10, 37, 300, 100), Qt::AlignLeft, "KeepAspectRatioByExpanding");
        break;
    }
}

void cvWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // M: changes aspect ratio mode
        case Qt::Key_M:
        {
            if (_ar_mode == Qt::IgnoreAspectRatio)
            {
                _ar_mode = Qt::KeepAspectRatio;
            }
            else if (_ar_mode == Qt::KeepAspectRatio)
            {
                _ar_mode = Qt::KeepAspectRatioByExpanding;
            }
            else
            {
                _ar_mode = Qt::IgnoreAspectRatio;
            }
        }

        // ESC: exit application
        case Qt::Key_Escape:
        {
            close();
        }
        break;
    }
}

void cvWindow::_open()
{
    // Display dialog so the user can select a file
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"),
                                                    QDir::currentPath(),
                                                    tr("Files (*.*)"));

    if (filename.isEmpty()) // Do nothing if filename is empty
        return;

    // If its already capturing, stop and release it!
    if (_capture)
    {
        _timer->stop();
        delete _timer;

        _capture->release();
        delete _capture;
    }

    // Create a new capture interface based on the filename
    _capture = new cv::VideoCapture(filename.toStdString());
    if (!_capture->isOpened())
    {
        qDebug() << "cvWindow::_open !!! Unable to open " << filename;
        setWindowTitle(tr("QT Video demo with OpenCV"));
        return;
    }

    // Set the filename as the window title
    setWindowTitle(filename);

    // Retrieve the width/height of the video. If not possible, then use the current size of the window
    _video_width = 0;
    _video_width = _capture->get(CV_CAP_PROP_FRAME_WIDTH);
    _video_height = 0;
    _video_height = _capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    qDebug() << "cvWindow::_open default size is " << _video_width << "x" << _video_height;

    if (!_video_width || !_video_height)
    {
        _video_width = width();
        _video_height = height();
    }

    // Resize the window to fit video dimensions
    resize(_video_width, _video_height);

    // Retrieve fps from the video. If not available, default will be 25
    _fps = 0;
    _fps = _capture->get(CV_CAP_PROP_FPS);
    qDebug() << "cvWindow::_open default fps is " << _fps;

    if (!_fps)
        _fps = 25;

    // Set FPS playback
    _tick_ms = 1000/_fps;

    // _image is created according to video dimensions
    if (_image)
    {
        delete _image;
    }
    _image = new QImage(_video_width, _video_height, QImage::Format_RGB888);

    // Start timer to read frames from the capture interface
    _timer = new QTimer();
    _timer->start(_tick_ms);
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}

void cvWindow::_close()
{
    qDebug() << "cvWindow::_close";
    emit closed();
}
