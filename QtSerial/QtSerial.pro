# Go to the Project properties,
# click on the Run tab,
# and check the box "Run in terminal".
CONFIG += console

QT += core serialport

SOURCES += \
    main.cpp \
    serialportreader.cpp

HEADERS += \
    serialportreader.h
