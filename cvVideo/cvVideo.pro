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

## OpenCV settings for Windows and OpenCV 2.4.2
win32 {
    INCLUDEPATH += "C:\\opencv\\build\\include" \
                   "C:\\opencv\\build\\include\\opencv" \
                   "C:\\opencv\\build\\include\\opencv2"

    LIBS += -L"C:\\opencv\\build\\x86\\vc10\\lib" \
        -lopencv_core242 \
        -lopencv_highgui242 \
        -lopencv_imgproc242
}

# Runs this app automatically after the building has succeeded
#QMAKE_POST_LINK=./$$TARGET
