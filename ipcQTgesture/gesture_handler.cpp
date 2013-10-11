#include "gesture_handler.h"

#include <iostream>

#include <QImage>


GestureHandler::GestureHandler()
: _streaming(false), _init_error(false),
_last_gesture(PXCGesture::Gesture::LABEL_ANY)
{
    QObject::connect(&_fps_timer, SIGNAL(timeout()), this, SLOT(_stream()));

    _pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB24);
    _pipeline.EnableGesture();

    if (!_pipeline.Init())
    {
        std::cout << "GestureHandler::GestureHandler !!! Failed initializing pipeline." << std::endl;
        _init_error = true;
    }
}

GestureHandler::~GestureHandler()
{
    if (_init_error)
        return;

    stop();
    _pipeline.Close();
    _pipeline.Release();
}

bool GestureHandler::start()
{
    if (_init_error)
        return false;

    if (!_streaming)
    {
        _streaming = true;
        _elapsed_timer.start();
        _fps_timer.start(1000/30);
    }

    return true;
}

void GestureHandler::stop()
{
    if (_init_error || !_streaming)
        return;

    _streaming = false;
    _fps_timer.stop();
}

QSize GestureHandler::image_size()
{
    if (_init_error)
        return QSize();

    pxcU32 img_w = 0, img_h = 0;
    if (!_pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_COLOR, img_w, img_h))
        return QSize();

    //std::cout << "GestureHandler::image_size Video Image size:" <<
    //             img_w << "x" << img_h << std::endl;

    return QSize(img_w, img_h);
}

/*
 *_stream() reads a single frame
 */
void GestureHandler::_stream()
{
    if (!_pipeline.AcquireFrame(true))    // true - blocking
        return;

    // Retrieve QueryImage() as PXCSmartPtr<PXCImage> will crash later on
    PXCImage* image = _pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);

    PXCImage::ImageData image_data;
    pxcStatus sts = image->AcquireAccess(PXCImage::ACCESS_READ,
                                         PXCImage::COLOR_FORMAT_RGB24,
                                         &image_data);
    if (sts < PXC_STATUS_NO_ERROR)
    {
        std::cout << "!!! Failed AcquireAccess." << std::endl;
        _pipeline.ReleaseFrame();
        return;
    }

    PXCImage::ImageInfo image_info;
    image->QueryInfo(&image_info);
    if (sts < PXC_STATUS_NO_ERROR)
    {
        std::cout << "!!! Failed QueryInfo." << std::endl;
        _pipeline.ReleaseFrame();
        return;
    }

    if (image_info.format == PXCImage::COLOR_FORMAT_RGB24)
    {   // BGR layout on little-endian machine
        unsigned char* bgr_img_data = (unsigned char*)image_data.planes[0];

        QImage temp(bgr_img_data,
                    image_info.width,
                    image_info.height,
                    QImage::Format_RGB888);

        // Convert data from BGR (PXCImage format) to RGB (QPixmap)
        QPixmap pixmap = QPixmap::fromImage(temp.rgbSwapped());
        emit notify_image(pixmap);
    }
    else
    {
        std::cout << "!!! TODO: support other image formats besides RGB24." << std::endl;
        _pipeline.ReleaseFrame();
        return;
    }

    image->ReleaseAccess(&image_data);
    if (sts < PXC_STATUS_NO_ERROR)
    {
        std::cout << "!!! Failed ReleaseAccess." << std::endl;
        _pipeline.ReleaseFrame();
        return;
    }

    /* Detect gestures */

    PXCGesture* gesture = _pipeline.QueryGesture();
    if (gesture)
    {
        // get the node data for the whole primary hand
        // (the one that shows up in the view first)
        PXCGesture::GeoNode node;
        sts = gesture->QueryNodeData(0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, &node);
        if (sts == PXC_STATUS_ITEM_UNAVAILABLE)
        {
            //std::cout << "!!! QueryNodeData: PXC_STATUS_ITEM_UNAVAILABLE" << std::endl;
            _pipeline.ReleaseFrame();
            return;
        }

        //std::cout << "* Hand at " << node.positionWorld.x << "x" << node.positionWorld.y << " z = " << node.positionWorld.z << std::endl;

        PXCGesture::Gesture gest;
        sts = gesture->QueryGestureData(0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, 0, &gest);
        if (sts == PXC_STATUS_ITEM_UNAVAILABLE)
        {
            //std::cout << "!!! QueryGestureData: PXC_STATUS_ITEM_UNAVAILABLE" << std::endl;
            _pipeline.ReleaseFrame();
            return;
        }

        if (gest.active)
        {
            // how many ms has passed since last gesture detection
            qint64 elapsed = _elapsed_timer.elapsed();

            switch (gest.label)
            {
                case PXCGesture::Gesture::LABEL_HAND_WAVE:
                case PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT:
                case PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT:
                case PXCGesture::Gesture::LABEL_NAV_SWIPE_UP:
                case PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN:
                case PXCGesture::Gesture::LABEL_HAND_CIRCLE:
                default:
                    emit notify_gesture(gest);
                break;

                case PXCGesture::Gesture::LABEL_POSE_THUMB_UP:
                    if ( (_last_gesture == PXCGesture::Gesture::LABEL_POSE_THUMB_UP && elapsed > 1000) ||
                          (_last_gesture != PXCGesture::Gesture::LABEL_POSE_THUMB_UP) )
                        emit notify_gesture(gest);
                break;

                case PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN:
                    if ( (_last_gesture == PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN && elapsed > 1000) ||
                        (_last_gesture != PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN) )
                        emit notify_gesture(gest);
                break;

                case PXCGesture::Gesture::LABEL_POSE_PEACE:
                    if ( (_last_gesture == PXCGesture::Gesture::LABEL_POSE_PEACE && elapsed > 1000) ||
                         (_last_gesture != PXCGesture::Gesture::LABEL_POSE_PEACE) )
                        emit notify_gesture(gest);
                break;

                case PXCGesture::Gesture::LABEL_POSE_BIG5:
                    if ( (_last_gesture == PXCGesture::Gesture::LABEL_POSE_BIG5 && elapsed > 1000) ||
                        (_last_gesture != PXCGesture::Gesture::LABEL_POSE_BIG5) )
                        emit notify_gesture(gest);
                break;
            }
            _last_gesture = gest.label;
            if (elapsed > 1000)
                _elapsed_timer.restart();
        }
    }

    _pipeline.ReleaseFrame();
}

