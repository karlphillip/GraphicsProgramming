## OpenCV settings for Windows and OpenCV 4.5.0
win32 {
    message("* Using settings for Windows.")
    INCLUDEPATH += "C:\\opencv\\build\\install\\include" \
                   "C:\\opencv\\build\\install\\include\\opencv2" \

    CONFIG(debug, debug | release) {
        LIBS += -L"C:\\opencv\\build\\install\\x64\\vc16\\lib" \
            -lopencv_core450d \
            -lopencv_highgui450d \
            -lopencv_imgproc450d \
            -lopencv_imgcodecs450d \
            -lopencv_videoio450d

        # Projects > Build & Run > Run
        # Check: add build library search path to PATH
        # This allows OpenCV DLLs to be found when running this application
        LIBS += -L"C:\\opencv\\build\\install\\x64\\vc16\\bin"
    }

    CONFIG(release, debug | release) {
        LIBS += -L"C:\\opencv\\build\\install\\x64\\vc16\\lib" \
            -lopencv_core450 \
            -lopencv_highgui450 \
            -lopencv_imgproc450 \
            -lopencv_imgcodecs450 \
            -lopencv_videoio450

        # Projects > Build & Run > Run
        # Check: add build library search path to PATH
        # This allows OpenCV DLLs to be found when running this application
        LIBS += -L"C:\\opencv\\build\\install\\x64\\vc16\\bin"
    }
}

SOURCES += \
    main.cpp

