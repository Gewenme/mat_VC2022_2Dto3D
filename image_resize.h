#pragma once
#include <opencv2/opencv.hpp>
#include <string>

namespace ImageResize {
    bool resizeImages(const std::string& inputFolder, const std::string& outputFolder, 
                     float scaleFactor = 1.0f, int targetWidth = 0, int targetHeight = 0);
    
    cv::Mat resizeImage(const cv::Mat& image, float scaleFactor = 1.0f, 
                       int targetWidth = 0, int targetHeight = 0);
}