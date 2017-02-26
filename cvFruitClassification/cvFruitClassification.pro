## OpenCV 3.0 settings for Windows
win32 {
    message("* Using settings for Windows.")

    INCLUDEPATH += "C:\\opencv\\build\\include" \
                   "C:\\opencv\\build\\include\\opencv" \
                   "C:\\opencv\\build\\include\\opencv2"

    CONFIG(debug, debug | release) {
        LIBS += -L"C:\\opencv\\build\\x86\\vc12\\lib" -lopencv_world300d
    }

    CONFIG(release, debug | release) {
        LIBS += -L"C:\\opencv\\build\\x86\\vc12\\lib" -lopencv_world300
    }

    QMAKE_CXXFLAGS += -O2
    QMAKE_CXXFLAGS += /MP
}

## OpenCV settings for Unix/Linux
unix:!mac {
    message("* Using settings for Unix/Linux.")
    INCLUDEPATH += /usr/local/include/opencv

    LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc
}

## OpenCV settings for Mac OS X
macx {
    message("* Using settings for Mac OS X.")
    INCLUDEPATH += /usr/local/include/opencv

    LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc
}

SOURCES += \
    main.cpp

HEADERS += \
    toolbox.h \
    dirent.h
