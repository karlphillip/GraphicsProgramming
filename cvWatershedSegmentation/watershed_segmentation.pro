## OpenCV settings for Windows and OpenCV 3.0 Alpha
win32 {
        message("* Using settings for Windows.")
    INCLUDEPATH += "C:\\opencv\\build\\include" \
                   "C:\\opencv\\build\\include\\opencv" \
                   "C:\\opencv\\build\\include\\opencv2"

    LIBS += -L"C:\\opencv\\build\\x86\\vc12\\lib" \
        -lopencv_world300d
}

SOURCES += \
    main.cpp

