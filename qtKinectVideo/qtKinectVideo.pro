# INSTALL
# -------
#
# 1- Microsoft Visual Studio 2013 Express (for Desktop, online installer)
#       https://www.visualstudio.com/downloads/download-visual-studio-vs
#
# 2- Qt 5.5.0 for Windows 32-bit (VS 2013, 633 MB)
#       http://download.qt.io/official_releases/qt/5.5/5.5.0/qt-opensource-windows-x86-msvc2013-5.5.0.exe
#
# 3- Kinect for Windows SDK v1.8 (222MB)
#	https://www.microsoft.com/en-us/download/confirmation.aspx?id=40278
#
# 4- Kinect for Windows Developer Toolkit v1.8 (384MB)
#	http://www.microsoft.com/en-us/download/confirmation.aspx?id=40276
#

QT += widgets

SOURCES += \
    main.cpp \
    window.cpp

win32 {
    INCLUDEPATH += "C:\\Program Files\\Microsoft SDKs\\Kinect\\v1.8\\inc"
    LIBS        += "C:\\Program Files\\Microsoft SDKs\\Kinect\\v1.8\\lib\\x86\\Kinect10.lib"
}

HEADERS += \
    window.h

