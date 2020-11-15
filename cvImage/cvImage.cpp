#include "cvImage.h"

#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


cvImage::cvImage()
: _menu(NULL), _image(NULL)
{
    setWindowTitle(tr("QT Image demo with OpenCV"));
    resize(480, 240);

    _menu = new QMenu("File");
    _menu->addAction("Open", this, SLOT(_open()));
    _menu->addSeparator();
    _menu->addAction("Exit", this, SLOT(close()));
    _menu_bar.addMenu(_menu);

    setMenuBar(&_menu_bar);
}

cvImage::~cvImage()
{
    if (_menu)
        delete _menu;
}

void cvImage::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    // When no image has been loaded, there's nothing to draw.
    if (_image.isNull())
        return;

    painter.drawImage(QPoint(0, 0), _image);

    QWidget::paintEvent(e);
}

void cvImage::_open()
{
    // Display dialog so the user can select a file
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"),
                                                    QDir::currentPath(),
                                                    tr("Files (*.png *.jpg *.tiff *.bmp)"));

    // do nothing if filename is empty
    if (filename.isEmpty())
        return;

    cv::Mat img = cv::imread(filename.toStdString(), -1);
    if (img.empty())
        return;

    switch (img.channels())
    {
        case 1:
        {
            _image = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8).copy();
            qDebug() << "cvImage::_open GRAYSCALE";
        }
        break;

        case 3:
        {
            // since OpenCV uses BGR order, we need to convert it to RGB
            cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
            _image = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).copy();
            qDebug() << "cvImage::_open RGB";
        }
        break;

        case 4:
        {
            _image = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_ARGB32).copy();
            qDebug() << "cvImage::_open RGBA";
        }
        break;

        default:
            qDebug() << "cvImage::_open !!! Unexpected number of channels: " <<  img.channels();
        return;

    }

    // Set the filename as the window title
    setWindowTitle(filename);

    // Resize the window to fit video dimensions
    resize(img.cols, img.rows);

    // Mouse move events will occur only when a mouse button is pressed down, unless mouse tracking has been enabled
    QWidget::setMouseTracking(true);

    // Trigger paint event to redraw the window
    update();
}

void cvImage::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // ESC: exit application
        case Qt::Key_Escape:
        {
            close();
        }
        break;
    }
}

void cvImage::mouseMoveEvent(QMouseEvent *event)
{
    if (_image.isNull())
        return;

    QPoint pos = event->pos();
    if (pos.x() < 0 || pos.x() >= _image.width())
        return;
    if (pos.y() < 0 || pos.y() >= _image.height())
        return;

    QString pos_x;
    pos_x = pos_x.setNum(pos.x());

    QString pos_y;
    pos_y = pos_y.setNum(pos.y());

    QColor pixel(_image.pixel(pos));
    int r = pixel.red();
    int g = pixel.green();
    int b = pixel.blue();

    QString pixel_info = pos_x + "," + pos_y + " R:" + QString().setNum(r) +
                                               " G:" + QString().setNum(g) +
                                               " B:" + QString().setNum(b);
    //qDebug() << "cvImage::mouseMoveEvent " <<  pixel_info;

    setWindowTitle(pixel_info);    
}

void cvImage::_close()
{
    emit closed();
}
