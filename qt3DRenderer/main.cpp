#include "window.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Window win;
    //win.resize(1280, 900);

    win.show();
    //win.showFullScreen();
    //win.showMaximized();

    return app.exec();
}
