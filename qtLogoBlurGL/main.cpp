/* Copyright (C) 2012-2020 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 *
 *
 * Ingress logo with glowing effect, based on Nehe lesson 36.
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

