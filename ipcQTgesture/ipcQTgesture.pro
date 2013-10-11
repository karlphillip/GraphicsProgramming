QT += core gui widgets

SOURCES += \
    ipc_window.cpp \
    main.cpp \
    gesture_handler.cpp

HEADERS += \
    ipc_window.h \
    gesture_handler.h

win32 {
    QMAKE_CXXFLAGS_DEBUG += -Zi -MTd
    QMAKE_CXXFLAGS_RELEASE += -MT

    INCLUDEPATH += "C:\\Program Files\\Intel\\PCSDK\\include" \
                   "C:\\Program Files\\Intel\\PCSDK\\sample\\common\\include" \
                   "C:\\Program Files\\Intel\\PCSDK\\sample\\common\\res"

    LIBS += -L"C:\\Program Files\\Intel\\PCSDK\\lib\\Win32" \
            -L"C:\\Program Files\\Intel\\PCSDK\\sample\\common\\lib\\Win32\\v100"

    CONFIG (debug, debug|release) {
        message("* Configuring project for Debug.")

        LIBS += -llibpxc_d \
                -llibpxcutils_d

        LIBS += "advapi32.lib" "msvcrtd.lib" "libcmtd.lib"
    }
    else {
        message("* Configuring project for Release.")

        LIBS += -llibpxc \
                -llibpxcutils

        LIBS += "advapi32.lib" "msvcrt.lib" "libcmt.lib"
    }

}

