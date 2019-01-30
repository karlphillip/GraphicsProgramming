SOURCES += \
    main.cpp

# On my system I have to specify g++ as compiler else it will use clang++ by default
#QMAKE_CXX=g++
#QMAKE_CC=gcc

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

## OpenCV settings for Windows and OpenCV 2.4.2
#win32 {
#    message("* Using settings for Windows.")
#    INCLUDEPATH += "C:\\opencv\\build\\include" \
#                   "C:\\opencv\\build\\include\\opencv" \
#                   "C:\\opencv\\build\\include\\opencv2"

#    CONFIG(debug, debug | release) {
#        LIBS += -L"C:\\opencv\\build\\x86\\vc10\\lib" \
#            -lopencv_core248d \
#            -lopencv_highgui248d \
#            -lopencv_imgproc248d
#    }

#    CONFIG(release, debug | release) {
#        LIBS += -L"C:\\opencv\\build\\x86\\vc10\\lib" \
#            -lopencv_core248 \
#            -lopencv_highgui248 \
#            -lopencv_imgproc248
#    }
#}

## OpenCV settings for Windows and OpenCV 3.4.1
win32 {
    message("* Using settings for Windows.")
    INCLUDEPATH += "C:\\opencv\\build\\include" \
                   "C:\\opencv\\build\\include\\opencv" \
                   "C:\\opencv\\build\\include\\opencv2"

    CONFIG(debug, debug | release) {
        LIBS += -L"C:\\opencv\\build\\vs2017\\install\\x86\\vc15\\lib" \
            -lopencv_world341d
    }

    CONFIG(release, debug | release) {
        LIBS += -L"C:\\opencv\\build\\vs2017\\install\\x86\\vc15\\lib" \
            -lopencv_world341
    }
}

# Runs this app automatically after the building has succeeded
#QMAKE_POST_LINK=./$$TARGET
