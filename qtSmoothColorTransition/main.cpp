#include <iostream>
#include <QApplication>
#include <QImage>
#include <QLabel>

// http://stackoverflow.com/a/25878225/176769
// http://mycodelog.com/2010/05/16/interpolation/
#if 0
QImage smooth_color_transition(const QImage& input, const QVector<QColor>& color_vec)
{
    QImage output(input.size(), input.format());

    QColor color_src = color_vec.at(0);
    unsigned int r1 = color_src.red();
    unsigned int g1 = color_src.green();
    unsigned int b1 = color_src.blue();

    QColor color_dst = color_vec.at(1);
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
#endif


QImage smooth_color_transition(const QImage& input, const QVector<QColor>& color_vec)
{
    if (color_vec.size() < 2)
        return QImage();

    QImage output(input.size(), input.format());

    int num_colors = color_vec.size();
    std::cout << "Num colors:" << color_vec.size() << std::endl;

    int color_width = 256 / (color_vec.size()-1);
    std::cout << "Color width:" << color_width << std::endl;

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

    std::cout << "Pallete size:" << pallete.size() << std::endl;

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

    QImage input("/Users/karlphillip/workspace/GraphicsProgramming/qtSmoothColorTransition/heightmap.jpg");
    if (input.isNull())
        return -1;

    QLabel input_label;
    input_label.setPixmap(QPixmap::fromImage(input));
    input_label.resize(input.width(), input.height());
    input_label.show();

    QVector<QColor> colors;
    colors.push_back(QColor(0, 120, 200));      // dark blue
    colors.push_back(QColor(120, 220, 255));    // cyan
    colors.push_back(QColor(71, 141, 104));     // dark green
    colors.push_back(QColor(106, 168, 107));    // medium green
    colors.push_back(QColor(141, 173, 132));    // light green
    colors.push_back(QColor(166, 191, 151));    // super light green
    colors.push_back(QColor(217, 233, 188));    // sand yellow
    colors.push_back(QColor(185, 140, 90));     // light orange
    colors.push_back(QColor(162, 90, 34));      // light red
    colors.push_back(QColor(144, 61, 45));      // dark red
    colors.push_back(QColor(151, 68, 64));      // super dark red
    colors.push_back(QColor(156, 146, 170));    // purple-red
    colors.push_back(QColor(170, 170, 210));    // purple-white
    colors.push_back(QColor(240, 247, 243));    // white

    QImage output = smooth_color_transition(input, colors);

    QLabel output_label;
    output_label.setPixmap(QPixmap::fromImage(output));
    output_label.resize(output.width(), output.height());
    output_label.show();

    output.save("colored.jpg");

    return app.exec();
}
