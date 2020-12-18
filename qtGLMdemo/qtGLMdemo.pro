QT += widgets opengl

LIBS += -lopengl32 -lGlU32

SOURCES += \
    glm.c \
    main.cpp \
    glwidget.cpp

HEADERS += \
    glm.h \
    glwidget.h
