#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace CornerDetection {
    struct CornerData {
        std::vector<cv::Point2f> corners;
        cv::Mat image;
        bool detected;
        int imageIndex;
    };
    
    bool detectAndDrawCorners(const std::string& inputFolder, const std::string& outputFolder,
                             int boardWidth, int boardHeight, float scaleFactor = 1.0f);
    
    bool detectChessboardCorners(const cv::Mat& image, int boardWidth, int boardHeight, 
                                std::vector<cv::Point2f>& corners);
    
    cv::Mat drawCornersWithNumbers(const cv::Mat& image, const std::vector<cv::Point2f>& corners, 
                                  int boardWidth, int boardHeight);
    
    bool saveCornerData(const std::vector<CornerData>& leftCorners, 
                       const std::vector<CornerData>& rightCorners,
                       const std::string& outputFile);
}