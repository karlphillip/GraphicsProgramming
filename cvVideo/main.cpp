#include "cvWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    cvWindow win;
    win.show();

    QObject::connect(&win, SIGNAL(closed()),
                     &app, SLOT(quit()) );

    return app.exec();
}
