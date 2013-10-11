#include "ipc_window.h"

#include <iostream>

#include <QApplication>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    IPCWindow* window = new IPCWindow();
    window->show();

    return app.exec();
}
