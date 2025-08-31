#pragma once
#include <opencv2/opencv.hpp>
#include <string>

namespace ModelViewer {
    bool displayPointCloud(const std::string& pointCloudFile);
    
    bool display3DModel(const cv::Mat& points3D, const cv::Mat& colors);
    
    bool saveVisualization(const cv::Mat& points3D, const cv::Mat& colors, 
                          const std::string& outputImage);
}