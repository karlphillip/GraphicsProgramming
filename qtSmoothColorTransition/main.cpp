/* Copyright (C) 2012-2020 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include <iostream>
#include <QApplication>
#include <QImage>
#include <QLabel>

/*
 * ColorMaps:
 *   http://jdherman.github.io/colormap/
 *   http://www.mathworks.com/matlabcentral/mlc-downloads/downloads/submissions/35242/versions/2/screenshot.jpg
 *   http://www.r-bloggers.com/gmt-standard-color-palettes/
 *
 * Linear interpolation:
 *   http://stackoverflow.com/a/25878225/176769
 *   http://mycodelog.com/2010/05/16/interpolation/
 */

// Artificial colorization of an image based in 2 starting colors
QImage smooth_color_transition(const QImage& input, const QColor color_src, const QColor color_dst)
{
    QImage output(input.size(), input.format());

    unsigned int r1 = color_src.red();
    unsigned int g1 = color_src.green();
    unsigned int b1 = color_src.blue();

    unsigned int r2 = color_dst.red();
    unsigned int g2 = color_dst.green();
    unsigned int b2 = color_dst.blue();

    for (int x = 0; x < input.width(); x++)
    {
        for (int y = 0; y < input.height(); y++)
        {
            QRgb pixel = input.pixel(x, y);
            int gray = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
            float p = gray / 255.0;

            //Resultant Color = RGB(A*(1-P)+X*P, B*(1-P)+Y*P, C*(1-P)+Z*P)
            int r = (int) r1 * (1-p) + r2 * p;
            int g = (int) g1 * (1-p) + g2 * p;
            int b = (int) b1 * (1-p) + b2 * p;
            output.setPixel(x, y, qRgb(r, g, b));            
        }
    }

    return output;
}

// Artificial colorization of an image based on color palette (vector)
QImage smooth_color_transition(const QImage& input, const QVector<QColor>& color_vec)
{
    if (color_vec.size() < 2)
        return QImage();

    QImage output(input.size(), QImage::Format_RGB888);

    int num_colors = color_vec.size();
    //std::cout << "Num colors:" << color_vec.size() << std::endl;

    int color_width = 256 / (color_vec.size()-1);
    //std::cout << "Color width:" << color_width << std::endl;

    QVector<QColor> pallete;
    for (int c = 1; c < num_colors; c++)
    {
        QColor color_src = color_vec.at(c-1);
        unsigned int r1 = color_src.red();
        unsigned int g1 = color_src.green();
        unsigned int b1 = color_src.blue();

        QColor color_dst = color_vec.at(c);
        unsigned int r2 = color_dst.red();
        unsigned int g2 = color_dst.green();
        unsigned int b2 = color_dst.blue();

        for (int i = 0; i < color_width; i++)
        {
            float p = i / (float)color_width; // percentage

            //Resultant Color = RGB(A*(1-P)+X*P, B*(1-P)+Y*P, C*(1-P)+Z*P)
            int r = (int) r1 * (1-p) + r2 * p;
            int g = (int) g1 * (1-p) + g2 * p;
            int b = (int) b1 * (1-p) + b2 * p;            
            pallete.push_back(qRgb(r, g, b));
        }
    }

    // Color width is a REAL number, therefore pallete might end up a little bit short
    for (int i = pallete.size(); i < 256; i++)
        pallete.push_back(color_vec[color_vec.size()-1].rgb());

    //std::cout << "Pallete size:" << pallete.size() << std::endl;

    for (int x = 0; x < input.width(); x++)
    {
        for (int y = 0; y < input.height(); y++)
        {
            QRgb pixel = input.pixel(x, y);
            int gray = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
            output.setPixel(x, y, pallete[gray].rgb());
        }
    }

    return output;
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QImage input("../qtSmoothColorTransition/heightmap.jpg");
    if (input.isNull())
    {
        std::cout << "!!! Unable to load image" << std::endl;
        return -1;
    }

    QLabel input_label;
    input_label.setPixmap(QPixmap::fromImage(input));
    input_label.resize(input.width(), input.height());
    input_label.show();

    QVector<QColor> colors;

    // Terrain
    colors.push_back(QColor(1, 1, 48));         // dark blue
    colors.push_back(QColor(29, 24, 245));      // blue
    colors.push_back(QColor(0, 255, 254));      // cyan
    colors.push_back(QColor(0, 100, 56));       // dark green
    colors.push_back(QColor(0, 127, 63));       // green-1
    colors.push_back(QColor(3, 153, 72));       // green-2
    colors.push_back(QColor(54, 178, 86));      // green-3
    colors.push_back(QColor(116, 204, 119));    // light green
    colors.push_back(QColor(173, 229, 161));    // light green-2
    colors.push_back(QColor(227, 255, 209));    // sand
    colors.push_back(QColor(186, 192, 107));    // dark sand
    colors.push_back(QColor(184, 90, 35));      // dark orange
    colors.push_back(QColor(105, 42, 0));       // dark red
    //colors.push_back(QColor(245, 245, 250));    // snow

/*
    // jet
    colors.push_back(QColor(0, 0, 191));
    colors.push_back(QColor(0, 0, 255));
    colors.push_back(QColor(0, 64, 255));
    colors.push_back(QColor(0, 128, 255));
    colors.push_back(QColor(0, 191, 255));
    colors.push_back(QColor(0, 255, 255));
    colors.push_back(QColor(64, 255, 191));
    colors.push_back(QColor(128, 255, 128));
    colors.push_back(QColor(64, 255, 191));
    colors.push_back(QColor(191, 255, 64));
    colors.push_back(QColor(255, 255, 0));
    colors.push_back(QColor(255, 191, 0));
    colors.push_back(QColor(255, 191, 0));
    colors.push_back(QColor(255, 128, 0));
    colors.push_back(QColor(255, 64, 0));
    colors.push_back(QColor(255, 0, 0));
    colors.push_back(QColor(191, 0, 0));
    colors.push_back(QColor(128, 64, 0));
*/

/*
    // BGR
    colors.push_back(QColor(0, 0, 255));
    colors.push_back(QColor(0, 255, 0));
    colors.push_back(QColor(255, 0, 0));
*/

/*
    // hot
    colors.push_back(QColor(43, 0, 0));
    colors.push_back(QColor(85, 0, 0));
    colors.push_back(QColor(128, 0, 0));
    colors.push_back(QColor(170, 0, 0));
    colors.push_back(QColor(213, 0, 0));
    colors.push_back(QColor(255, 0, 0));
    colors.push_back(QColor(255, 43, 0));
    colors.push_back(QColor(255, 85, 0));
    colors.push_back(QColor(255, 128, 0));
    colors.push_back(QColor(255, 170, 0));
    colors.push_back(QColor(255, 213, 0));
    colors.push_back(QColor(255, 255, 0));
    colors.push_back(QColor(255, 255, 64));
    colors.push_back(QColor(255, 255, 128));
    colors.push_back(QColor(255, 255, 191));
    colors.push_back(QColor(255, 255, 255));
*/

/*
    // cool
    colors.push_back(QColor(0, 255, 255));
    colors.push_back(QColor(17, 238, 255));
    colors.push_back(QColor(34, 221, 255));
    colors.push_back(QColor(51, 204, 255));
    colors.push_back(QColor(68, 187, 255));
    colors.push_back(QColor(85, 170, 255));
    colors.push_back(QColor(102, 153, 255));
    colors.push_back(QColor(68, 187, 255));
    colors.push_back(QColor(85, 170, 255));
    colors.push_back(QColor(102, 153, 255));
    colors.push_back(QColor(119, 136, 255));
    colors.push_back(QColor(136, 119, 255));
    colors.push_back(QColor(153, 102, 255));
    colors.push_back(QColor(170, 85, 255));
    colors.push_back(QColor(187, 68, 255));
    colors.push_back(QColor(204, 51, 255));
    colors.push_back(QColor(221, 34, 255));
    colors.push_back(QColor(238, 17, 255));
    colors.push_back(QColor(255, 0, 255));
*/

/*
    // water temperature
    colors.push_back(QColor(41, 10, 216));
    colors.push_back(QColor(38, 77, 255));
    colors.push_back(QColor(63, 160, 255));
    colors.push_back(QColor(114, 217, 255));
    colors.push_back(QColor(140, 247, 255));
    colors.push_back(QColor(224, 255, 255));
    colors.push_back(QColor(255, 255, 191));
    colors.push_back(QColor(255, 224, 153));
    colors.push_back(QColor(255, 255, 191));
    colors.push_back(QColor(255, 173, 114));
    colors.push_back(QColor(247, 109, 94));
    colors.push_back(QColor(216, 38, 50));
    colors.push_back(QColor(164, 0, 33));
*/

    // Build colored elevation map
    QImage output = smooth_color_transition(input, colors);
    output.save("../qtSmoothColorTransition/output.jpg");

    QLabel output_label;
    output_label.setPixmap(QPixmap::fromImage(output));
    output_label.resize(output.width(), output.height());
    output_label.show();

    return app.exec();
}
