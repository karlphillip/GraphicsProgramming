TEMPLATE = app

QT      += core gui

HEADERS += \
    cvWindow.h

SOURCES += \
    main.cpp \
    cvWindow.cpp

## OpenCV settings for Mac OS X
macx {
    INCLUDEPATH += /usr/local/include/opencv

    LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc
}

# Runs this app automatically after the building has succeeded
#QMAKE_POST_LINK=./$$TARGET
