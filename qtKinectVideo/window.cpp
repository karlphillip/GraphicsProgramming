/* Copyright (C) 2015 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "window.h"

#include <iostream>

#include <QApplication>


Window::Window()
: _tick_ms(33), _ar_mode(Qt::IgnoreAspectRatio), _fps(0),
  _video_width(640), _video_height(480), _timer(NULL), _nui_sensor(NULL)
{
    setWindowTitle("QT demo with Kinect");
    resize(_video_width, _video_height);

    // Initialize device and prepare it to send RGB data
    if (!_initKinect())
    {
        std::cout << "* FAILED! Is your Kinect device connected? Is it powered?" << std::endl;
        return;
    }

    std::cout << "* Special keys: " << std::endl;
    std::cout << "\tESC    - Quit application" << std::endl;
    std::cout << "\tM      - Change aspect ratio" << std::endl;
    std::cout << "\tUp     - Increase elevation" << std::endl;
    std::cout << "\tDown   - Decrease elevation" << std::endl;
    std::cout << "\tLEFT   - Reset elevation to 0 degrees" << std::endl;

    // Start timer to read frames from Kinect
    _timer = new QTimer();
    _timer->start(_tick_ms);
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}

Window::~Window()
{
    if (_timer)
    {
        _timer->stop();
        delete _timer;
    }

    // Shutdown Kinect properly
    if (_nui_sensor)
        _nui_sensor->NuiShutdown();
}

/* _tick(): called every few milliseconds.
 * It reads data from Kinect's capture interface and saves the frame as QImage.
 */
void Window::_tick()
{
    if (!_nui_sensor)
    {
        std::cout << "_tick !!! _initKinect() failed: ABORT!" << std::endl;
        return;
    }

    /* Read data from Kinect */

    NUI_IMAGE_FRAME _nui_frame;
    if (_nui_sensor->NuiImageStreamGetNextFrame(_rgb_stream, 0, &_nui_frame) < 0)
        return;

    NUI_LOCKED_RECT locked_rect;
    INuiFrameTexture* texture = _nui_frame.pFrameTexture;
    HRESULT result = texture->LockRect(0, &locked_rect, NULL, 0);
    if (result == S_OK && locked_rect.Pitch != 0)
    {
        NUI_SURFACE_DESC surface_desc;
        texture->GetLevelDesc(0, &surface_desc);
        if (_video_width != surface_desc.Width || _video_height != surface_desc.Height)
        {
            _video_width = surface_desc.Width;
            _video_height = surface_desc.Height;
            resize(_video_width, _video_height);
        }

        // Copy pixels to QImage
        _image = QImage((const uchar*)locked_rect.pBits,
                        surface_desc.Width, surface_desc.Height,
                        QImage::Format_RGB32);


        // Trigger paint event to redraw the window
        if (!_image.isNull())
            emit update();
    }

    texture->UnlockRect(0);

    _nui_sensor->NuiImageStreamReleaseFrame(_rgb_stream, &_nui_frame);
}

bool Window::_initKinect()
{
    int sensor_count = 0;
    if (NuiGetSensorCount(&sensor_count) < 0 || sensor_count < 1)
    {
        std::cout << "_initKinect !!! NuiGetSensorCount() failed" << std::endl;
        return false;
    }

    std::cout << "* Number of compatible sensors detected: " << sensor_count << std::endl;

    /* Look at each Kinect sensor */

    bool sensor_found = false;
    for (int i = 0; i < sensor_count; i++)
    {
        std::cout << "* Checking sensor #" << i << " ... ";

        // Create the sensor so we can check status, if we can't create it, move on to the next
        if (NuiCreateSensorByIndex(i, &_nui_sensor) < 0)
        {
            std::cout << " SKIPPED!" << std::endl;
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        if (_nui_sensor->NuiStatus() != S_OK)
        {
            std::cout << " NOT READY!" << std::endl;
        }
        else
        {
            std::cout << " SELECTED!" << std::endl;
            sensor_found = true;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        _nui_sensor->Release();
    }    

    if (_nui_sensor == NULL || !sensor_found)
    {        
        std::cout << "_initKinect !!! No ready Kinect found." << std::endl;
        return false;
    }   

    // Initialize the Kinect and specify that we'll be using color
    if (_nui_sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR) >= 0) // if (SUCCEEDED(hr))
    {
        // Open a color image stream to receive color frames
        if (_nui_sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,           // Depth camera or rgb camera?
                                             NUI_IMAGE_RESOLUTION_640x480,  // Image resolution
                                             0,                             // Image stream flags, e.g. near mode
                                             2,                             // Number of frames to buffer
                                             NULL,                          // Event handle
                                             &_rgb_stream) < 0)
        {
            std::cout << "_initKinect !!! NuiImageStreamOpen() failed" << std::endl;
            return false;
        }
    }

    return true;
}

void Window::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);

    // Draw a frame
    QPainter painter(this);
    _draw_video_frame(painter);
}

void Window::_draw_video_frame(QPainter& painter)
{
    // When no image is loaded, paint a black frame
    if (_image.isNull())
    {
        // QImage is not ready yet
        painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::black);
        return;
    }

    // To simply draw on the window, execute the code below.
    //painter.drawImage(QPoint(0, 0), _image);

    // But we are drawing the image according to AR settings:
    // http://doc.qt.digia.com/stable/qt.html#AspectRatioMode-enum
    switch (_ar_mode)
    {
        default:
        case Qt::IgnoreAspectRatio:
        {
            painter.drawImage(QRectF(QPoint(0, 0), QSize(_video_width, _video_height)),
                              _image,
                              QRectF(QPoint(0, 0), _image.size()));
        }
        break;

        case Qt::KeepAspectRatio:
        {
            painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::black);

            QImage scaled_img = _image.scaled(QSize(width(), height()), Qt::KeepAspectRatio, Qt::FastTransformation);

            painter.drawImage(qRound(width()/2.0)  - qRound(scaled_img.size().width()/2.0),
                              qRound(height()/2.0) - qRound(scaled_img.size().height()/2.0),
                              scaled_img);
        }
        break;

        case Qt::KeepAspectRatioByExpanding:
        {
            QImage scaled_img = _image.scaled(QSize(width(), height()), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

            painter.drawImage(QRectF(QPoint(0, 0), QSize(width(), height())),
                               scaled_img,
                               QRectF(QPoint(qRound(scaled_img.size().width()/2.0) - qRound(width()/2.0),
                                             qRound(scaled_img.size().height()/2.0) - qRound(height()/2.0)),
                                             QSize(width(), height())));
        }
        break;
    }
}

void Window::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // ESC: exit application
        case Qt::Key_Escape:
        {
            std::cout << "* (ESC) Bye bye." << std::endl;
            QApplication::instance()->quit();
        }
        break;

        // M: changes aspect ratio mode
        case Qt::Key_M:
        {
            if (_ar_mode == Qt::IgnoreAspectRatio)
            {
                _ar_mode = Qt::KeepAspectRatio;
                std::cout << "* (M) AR = keep aspect ratio" << std::endl;
            }
            else if (_ar_mode == Qt::KeepAspectRatio)
            {
                _ar_mode = Qt::KeepAspectRatioByExpanding;
                std::cout << "* (M) AR = keep aspect ratio by expanding" << std::endl;
            }
            else
            {
                _ar_mode = Qt::IgnoreAspectRatio;
                std::cout << "* (M) AR = ignore aspect ratio" << std::endl;
            }
        }
        break;

        // Up arrow: increases elevation angle by 2 degrees
        case Qt::Key_Up:
        {
            long angle = -1;
            if (NuiCameraElevationGetAngle(&angle) >= 0)
            {
                angle += 2;
                if (NuiCameraElevationSetAngle(angle) >= 0)
                    std::cout << "* (UP) Current elevation angle: " << angle << std::endl;
            }
        }
        break;

        // Down arrow: decreases elevation angle by 2 degrees
        case Qt::Key_Down:
        {
            long angle = -1;
            if (NuiCameraElevationGetAngle(&angle) >= 0)
            {
                angle -= 2;
                if (NuiCameraElevationSetAngle(angle) >= 0)
                    std::cout << "* (DOWN) Current elevation angle: " << angle << std::endl;
            }
        }
        break;

        // Left arrow: reset elevation angle
        case Qt::Key_Left:
        {
            long angle = -1;
            if (NuiCameraElevationGetAngle(&angle) >= 0)
            {
                angle = 0;
                if (NuiCameraElevationSetAngle(angle) >= 0)
                    std::cout << "* (LEFT) Current elevation angle: " << angle << std::endl;
            }
        }
        break;

    }
}
