#include <QApplication>
#include "glwidget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GLWidget gl_widget;
    gl_widget.show();

    return app.exec();
}
