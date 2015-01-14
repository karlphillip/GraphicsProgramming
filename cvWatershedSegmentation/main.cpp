/* Copyright (C) 2014 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 *
 *
 * A simple Watershed segmentation example, as described at:
 * http://stackoverflow.com/a/25851951/176769
 */
#include <iostream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main()
{
    cv::Mat img = cv::imread("beans.png");
    if (img.empty())
    {
        std::cout << "!!! Failed to open image" << std::endl;
        return -1;
    }

    /* Pre-processing */

    // Convert input image to grayscale
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    // Threshold the image and retrieve its binary representation
    cv::Mat img_bin;
    cv::threshold(img_gray, img_bin, 128, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
    cv::imwrite("img_bin.png", img_bin);

    // Remove small objects (noises)
    cv::Mat three_by_three_element(5, 5, CV_8U, cv::Scalar(1));
    cv::morphologyEx(img_bin, img_bin, cv::MORPH_OPEN, three_by_three_element);
    cv::imwrite("img_bin_morphoEx.png", img_bin);

    /* Processing */

    // Get the borders of the objects (beans)
    cv::Mat dilation, erosion;
    cv::dilate(img_bin, dilation, cv::Mat(), cv::Point(-1,-1), 3);
    cv::erode(dilation, erosion, cv::Mat());
    cv::Mat border(img_bin.size(), CV_8U, cv::Scalar(0));
    border = dilation - erosion;
    cv::imwrite("border.png", border);

    // Get the distance transform and normalize the result to [0,255]
    cv::Mat dt;
    cv::distanceTransform(img_bin, dt, cv::DIST_L2, 5);
    cv::normalize(dt, dt, 0, 255, cv::NORM_MINMAX);

    // Threshold it to isolate the peaks
    cv::threshold(dt, dt, 135, 255, cv::THRESH_BINARY);
    cv::imwrite("dt_thres.png", dt);

    // Use connectedComponents() to isolate the objects
    dt.convertTo(dt, CV_8U);
    cv::Mat lbl(dt.size(), CV_32S);
    int ncc = cv::connectedComponents(dt, lbl, 4);
    //std::cout << "Number of Connected Components: " << ncc << std::endl;

    // Create a gray color spectrum to paint the peaks
    std::vector<unsigned char> colors(ncc);
    colors[0] = 0; // for background
    for(int label = 1; label < ncc; ++label)
        colors[label] = 255/ncc*label;

    // Paint every peak (bean candidate) with a shade of gray
    for (int x = 0; x < dt.rows; x++) {
        for (int y = 0; y < dt.cols; y++) {
            int label = lbl.at<int>(x, y);
            dt.at<unsigned char>(x, y) = colors[label];
         }
    }
    cv::imwrite("conn_comp.png", dt);

    // Create the marker image for watershed
    cv::Mat markers = cv::Mat::zeros(dt.size(), CV_8U);
    markers = dt + border;
    cv::imwrite("markers.png", markers);

    markers.convertTo(markers, CV_32SC1);
    cv::watershed(img, markers);

    markers.convertTo(markers, CV_8U);
    markers = 255 - markers; // cv::bitwise_not()
    cv::imwrite("watershed.png", markers);

    // Create a green mask with the sillhuete of the detected objects
    cv::Mat green_mask = cv::Mat::zeros(markers.size(), CV_8UC3);
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            // Draw the marker's silhuete (white) as green in the mask
            if (markers.at<unsigned char>(i,j) == 255)
                green_mask.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 255, 0);
        }
    }

    cv::dilate(green_mask, green_mask, cv::Mat());
    cv::imshow(std::string("result"), green_mask);

    cv::Mat result = img + green_mask;
    cv::imshow(std::string("result"), result);
    cv::imwrite("result.png", result);

    cv::waitKey(0);

    return 0;
}
