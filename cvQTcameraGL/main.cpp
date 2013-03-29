#include <QApplication>
#include "widget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // For OpenGL Graphics
    GLWidget gl_widget;
    gl_widget.show();

    return app.exec();
}

