/* This application demonstrates how to train and use LDA to classify images
 * into Apples or Oranges.
 *
 * This work is mostly a C++ translation to OpenCV of FruitClassification,
 * a project written in Python by Eliezer Bernart, available here:
 *     https://github.com/eliezerb/FruitClassification
 */
#include "toolbox.h"

std::vector<Features> load_training_set(std::string path)
{
    std::vector<Features> features_vec;

    std::vector<std::string> filenames = ls_dir(path);
    for (unsigned int i = 0; i < filenames.size(); i++)
    {
        Features f = get_features(filenames[i]);
        if (f.aspect_ratio == 0 && f.b_color == 0 && f.g_color == 0 && f.r_color == 0)
            continue;

        features_vec.push_back(f);
    }

    return features_vec;
}

int main()
{
    /* Step 1: Process all images in apples/oranges directories
     * in order to extract their features.
     *
     * The feature vector is made of the following 4 attributes:
     *   - The aspect ratio of the fruit (width / height);
     *     The most dominant color of the fruit, which consists of:
     *      - The average Red color of the fruit;
     *      - The average Green color of the fruit;
     *      - The average Blue color of the fruit;
     */

    // x_samples contains all the Apples' features for training
    std::vector<Features> x_samples = load_training_set("../cvFruitClassification/apples");

    // y_samples contains all the Orange's features for training
    std::vector<Features> y_samples = load_training_set("../cvFruitClassification/oranges");


    /* Step 2: Create a Mat structure to store the training data */

    unsigned int num_rows = static_cast<unsigned int>(x_samples.size() + y_samples.size());
    cv::Mat train_data;
    for (unsigned int i = 0; i < x_samples.size(); i++)
    {
        // Each loop converts a Feature to a Mat's row
        cv::Mat row = cv::Mat::zeros(1, 4, CV_64FC1);
        row.at<double>(0) = x_samples[i].aspect_ratio;
        row.at<double>(1) = x_samples[i].b_color;
        row.at<double>(2) = x_samples[i].g_color;
        row.at<double>(3) = x_samples[i].r_color;
        train_data.push_back(row);
    }

    for (unsigned int i = 0; i < y_samples.size(); i++)
    {
        // Each loop converts a Feature to a Mat's row
        cv::Mat row = cv::Mat::zeros(1, 4, CV_64FC1);
        row.at<double>(0) = y_samples[i].aspect_ratio;
        row.at<double>(1) = y_samples[i].b_color;
        row.at<double>(2) = y_samples[i].g_color;
        row.at<double>(3) = y_samples[i].r_color;
        train_data.push_back(row);
    }


    /* Step 3: create labels for each training data
     *   - 0 represents the Apple class
     *   - 1 represents the Orange class
     *
     * Since the first 10 samples were Apples and the
     * next 10 samples where Orange, let's make this simple!
     */

    int apple_class = 0;
    int orange_class = 1;
    std::vector<int> labels;
    for (unsigned int i = 0; i < num_rows; i++)
    {
        if (i < x_samples.size())
            labels.push_back(apple_class);
        else
            labels.push_back(orange_class);
    }

    /* Step 4: train LDA */

    int num_components = 2; // there's only 2 classes anyway
    cv::LDA lda(train_data, labels, num_components);
    cv::Mat projected = lda.project(train_data);


    /* Step 5: test the classifier */

    std::vector<Features> t_samples = load_training_set("../cvFruitClassification/test");
    for (unsigned int i = 0; i < t_samples.size(); i++)
    {
        // Each loop converts a Feature to a Mat's row
        cv::Mat test_data = cv::Mat::zeros(1, 4, CV_64FC1);
        test_data.at<double>(0) = t_samples[i].aspect_ratio;
        test_data.at<double>(1) = t_samples[i].b_color;
        test_data.at<double>(2) = t_samples[i].g_color;
        test_data.at<double>(3) = t_samples[i].r_color;

        // Project the new data and check its prediction
        cv::Mat proj_class = lda.project(test_data);

        int min_class = -1;
        double best_dist = 999999999.9;
        for (int x = 0; x < projected.rows; x++)
        {
            double dist = cv::norm(projected.row(x), proj_class);
            if (dist < best_dist)
            {
                best_dist = dist;
                min_class = labels[x];
            }
        }

        std::cout << t_samples[i].filename << " belongs to class " << min_class << " -> " <<
                     ((min_class==0) ? "Apple" : "Orange") << std::endl;
    }

    return 0;
}
