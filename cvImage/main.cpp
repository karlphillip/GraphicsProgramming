#include "cvImage.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    cvImage win;
    win.show();

    QObject::connect(&win, SIGNAL(closed()),
                     &app, SLOT(quit()) );

    return app.exec();
}

