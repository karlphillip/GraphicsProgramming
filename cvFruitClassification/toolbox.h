#pragma once
#include "dirent.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class Features
{
public:
    std::string filename; // for debug purposes only

    // features
    float aspect_ratio = 0.f;
    unsigned char b_color = 0;
    unsigned char g_color = 0;
    unsigned char r_color = 0;
};

void segment_fruit(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largest_contour)
{
    std::vector<cv::Mat> bgr;
    cv::split(src ,bgr);

    cv::Mat thres_R, thres_G, thres_B;
    cv::threshold(bgr[0], thres_B, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    cv::threshold(bgr[1], thres_G, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    cv::threshold(bgr[2], thres_R, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    cv::Mat segmented = thres_B | thres_G | thres_R;

    int w = src.size().width;
    int h = src.size().height;
    cv::Mat mask = cv::Mat::zeros(cv::Size(w+2, h+2), CV_8U);

    cv::Mat filled = segmented.clone();
    cv::floodFill(filled, mask, cv::Point(0,0), cv::Scalar(255));
    cv::Mat segmentation_filled = segmented | ~filled;

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(segmentation_filled, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    double largest_area = 0;
    int largest_contour_idx = 0;
    for (unsigned int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i], false);
        if (area > largest_area) {
            largest_area = area;
            largest_contour_idx = i;
        }
    }

    cv::Mat final_segmentation(src.size(), CV_8U, cv::Scalar(0));
    cv::drawContours(final_segmentation, contours, largest_contour_idx, cv::Scalar(255, 255, 255), cv::FILLED, 4, hierarchy);

    dst = final_segmentation .clone();
    largest_contour = contours[largest_contour_idx];
}

cv::Scalar get_dominant_color(const cv::Mat& src, const cv::Mat& segmented)
{
    cv::Mat mask = cv::Mat::zeros(cv::Size(src.size().width, src.size().height), CV_8U) + segmented;
    return cv::mean(src, mask);
}

float get_fruit_structure(const std::vector<cv::Point>& obj_contour)
{
    cv::Rect rect = cv::boundingRect(obj_contour);
    return (rect.width / (float)rect.height);
}

Features get_features(const std::string& file)
{
    cv::Mat src = cv::imread(file, cv::IMREAD_COLOR);
    if (!src.data)
    {
        std::cout << "get_features !!! Failed to open: " << file << std::endl ;
        return Features();
    }

    cv::Mat segmented;
    std::vector<cv::Point> obj_contour;
    segment_fruit(src, segmented, obj_contour);

    cv::Scalar mean_color = get_dominant_color(src, segmented);

    float ar = get_fruit_structure(obj_contour);

    Features f;
    f.filename = file;
    f.b_color = (unsigned char)mean_color[0];
    f.g_color = (unsigned char)mean_color[1];
    f.r_color = (unsigned char)mean_color[2];
    f.aspect_ratio = ar;

    return f;
}

std::vector<std::string> ls_dir(const std::string& path)
{
    std::vector<std::string> filenames;

    DIR* dir = opendir(path.c_str());
    if (dir == NULL)
    {
        std::cout << "ls_dir !!! Directory not found: " << path << std::endl;
        return filenames;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name(reinterpret_cast<char*>(entry->d_name), strlen(entry->d_name));
        if (name == "." || name == "..")
            continue;

        std::string fullpath = path + "\\" + name;
        filenames.push_back(fullpath);
    }

    closedir(dir);

    return filenames;
}
