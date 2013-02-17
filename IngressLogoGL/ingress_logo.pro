QT += core opengl

contains(QT_VERSION, ^5\\.[0-8]\\..*) {
	message("* Using Qt $${QT_VERSION}.")
	QT += widgets

	# On my system I have to specify g++ as compiler else it will use clang++ by default
	QMAKE_CXX=g++
	QMAKE_CC=gcc
}

unix {
	INCLUDEPATH += /usr/X11/include 
} 

SOURCES += \
    main.cpp \
    GLwindow.cpp

HEADERS += \
    GLwindow.h
