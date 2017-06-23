#include "Madplotlib.h"

#include <iostream>

#include <QApplication>
#include <Eigen/Dense>
#include <QStandardItem>

/* 3.2: Linear Regression with Ordinary Least Squares Part 1 - Intelligence and Learning
 * https://www.youtube.com/watch?v=szXbuO3bVRk
 */
void linearRegression(const Eigen::ArrayXf& x, const Eigen::ArrayXf& y)
{
    // Plot original data as scatter plot
    Madplotlib plt;
    plt.xlabel("X");
    plt.ylabel("Y");
    plt.plot(x, y, QString("o"), QColor(0x0000FF));

    /*
     * x_mean = Σ(xi) / n
     * y_mean = Σ(yi) / n
     */

    float x_mean = x.sum() / x.rows();  // x.mean()
    float y_mean = y.sum() / y.rows();  // y.mean()

    /*
     * m = Σ(xi - x_mean)(yi - y_mean)      -> numerator
     *      ---------------------------
     *          Σ(xi - x_mean)^2            -> denominator
     */

    // make sure x and y are of the same size!
    float numerator = 0;
    float denominator = 0;
    for (int i = 0; i < x.cols(); i++)
    {
        numerator += (x[i] - x_mean) * (y[i] - y_mean);
        denominator += (x[i] - x_mean) * (x[i] - x_mean);
    }

    // TODO: protect it against div by 0?
    float m = numerator / denominator;  // m = slope

    /*
     * b = y_mean - m * xmean
     */
    float b = y_mean - m * x_mean;      // b = y intercept

    std::cout << "numerator: " << numerator << std::endl;
    std::cout << "denominator: " << denominator << std::endl;
    std::cout << "m: " << m << std::endl;
    std::cout << "b: " << m << std::endl;

    /* Plot the line of best fit:
     * The following x values are the smallest and largest numbers of the input vector.
     */

    float x1 = x.minCoeff(); // 0
    float y1 = m * x1 + b;

    float x2 = x.maxCoeff();
    float y2 = m * x2 + b;

    std::cout << "min x value: " << x1 << std::endl;
    std::cout << "max x value: " << x2 << std::endl;

    Eigen::ArrayXf x_coords(2);
    x_coords << x1, x2;

    Eigen::ArrayXf y_coords(2);
    y_coords << y1, y2;

    plt.plot(x_coords, y_coords, QColor(0xFF0000));
    plt.show();
    plt.savefig("linearRegression.png");
}

/* 3.4: Linear Regression with Gradient Descent - Intelligence and Learning
 * Stochastic Gradient Descent Implementation:
 *   Adjusting weights (m, b) with every data point.
 *
 * https://www.youtube.com/watch?v=L-Lsfu4ab74&list=PLRqwX-V7Uu6YJ3XfHhT2Mm4Y5I99nrIKX&index=43
 */
void linearRegressionSGD(const Eigen::ArrayXf& x_data, const Eigen::ArrayXf& y_data)
{
    // Plot original data as scatter plot
    Madplotlib plt;
    plt.xlabel("X");
    plt.ylabel("Y");
    plt.plot(x_data, y_data, QString("o"), QColor(0x0000FF));

    float learning_rate = 0.001f;
    float m = 0, b = 0;
    for (int i = 0; i < x_data.cols(); i++)
    {
        float x = x_data[i];
        float y = y_data[i];
        float guess = m * x + b;
        float error = y - guess;
        m = m + (error * x * learning_rate);
        b = b + (error * learning_rate);
    }

    // If you want a line from the beggining to the end of the chart,
    // make sure x1 is 0 and x2 is 50
    float x1 = x_data.minCoeff();
    float y1 = m * x1 + b;

    float x2 = x_data.maxCoeff();
    float y2 = m * x2 + b;

    std::cout << "line from (" << x1 << ", " << y1 <<
                 ") to (" << x2 << ", " << y2 << ")" << std::endl;
    std::cout << "m: " << m << std::endl;
    std::cout << "b: " << b << std::endl;

    Eigen::ArrayXf x_coords(2);
    x_coords << x1, x2;

    Eigen::ArrayXf y_coords(2);
    y_coords << y1, y2;

    plt.plot(x_coords, y_coords, QColor(0xFF0000));
    plt.show();
    plt.savefig("linearRegressionSGD.png");
}

void test1()
{
    std::cout << "* Test 1" << std::endl;

    /* http://www.itl.nist.gov/div898/strd/lls/lls.shtml
     * http://www.itl.nist.gov/div898/strd/lls/data/LINKS/DATA/Norris.dat
     *
     *  Calibration of Ozone Monitors.
     */

    Eigen::ArrayXf x(36);
    x << 0.2,
         337.4,
         118.2,
         884.6,
         10.1,
         226.5,
         666.3,
         996.3,
         448.6,
         777,
         558.2,
         0.4,
         0.6,
         775.5,
         666.9,
         338,
         447.5,
         11.6,
         556,
         228.1,
         995.8,
         887.6,
         120.2,
         0.3,
         0.3,
         556.8,
         339.1,
         887.2,
         999,
         779,
         11.1,
         118.3,
         229.2,
         669.1,
         448.9,
         0.5;

    Eigen::ArrayXf y(36);
    y << 0.1,
         338.8,
         118.1,
         888,
         9.2,
         228.1,
         668.5,
         998.5,
         449.1,
         778.9,
         559.2,
         0.3,
         0.1,
         778.1,
         668.8,
         339.3,
         448.9,
         10.8,
         557.7,
         228.3,
         998,
         888.8,
         119.6,
         0.3,
         0.6,
         557.6,
         339.3,
         888,
         998.5,
         778.9,
         10.2,
         117.6,
         228.9,
         668.4,
         449.2,
         0.2;

    linearRegression(x, y);
}


void test2()
{
    std::cout << "* Test 2" << std::endl;

    /* http://college.cengage.com/mathematics/brase/understandable_statistics/7e/students/datasets/slr/frames/frame.html
     *
     * National Unemployment Male Vs. Female
     * X = national unemployment rate for adult males
     * Y = national unemployment rate for adult females
     */

    Eigen::ArrayXf x(13);
    x << 2.900000095,
         6.699999809,
         4.900000095,
         7.900000095,
         9.800000191,
         6.900000095,
         6.099999905,
         6.199999809,
         6,
         5.099999905,
         4.699999809,
         4.400000095,
         5.800000191;


    Eigen::ArrayXf y(13);
    y << 4,
         7.400000095,
         5,
         7.199999809,
         7.900000095,
         6.099999905,
         6,
         5.800000191,
         5.199999809,
         4.199999809,
         4,
         4.400000095,
         5.199999809;

    linearRegression(x, y);
}


void test3()
{
    std::cout << "* Test 3" << std::endl;

    Eigen::ArrayXf x(12);
    x << 30, 33,
         32, 34,
         35, 37,
         38, 41,
         44, 46,
         44, 49;

    Eigen::ArrayXf y(12);
    y << 12, 12,
         13, 13,
         14, 14,
         15, 15,
         16, 16,
         17, 17;

    linearRegressionSGD(x, y);
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    test1();      // simple Linear Regression (LR)
    test2();      // simple Linear Regression (LR)
    test3();      // LR with Stochastic Gradient Descent #1

    std::cout << "* Done!" << std::endl;
    return 0;
}
