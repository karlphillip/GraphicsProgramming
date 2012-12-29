/*     main.cpp
 *
 *     Karl Phillip Buhr, 2012, 2013
 *     karlphillip@gmail.com
 *     http://stackoverflow.com/users/176769/karlphillip
 *
 *     Ingress logo with glowing effect, based on Nehe lesson 36.
 */
#include <QApplication>
#include "GLwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GLwindow gl_win;
    gl_win.show();

    return app.exec();
}

