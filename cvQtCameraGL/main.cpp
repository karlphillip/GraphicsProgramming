/* Copyright (C) 2013-2020 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
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

