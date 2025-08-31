#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace MonoCalibration {
    struct CalibrationResult {
        cv::Mat cameraMatrix;
        cv::Mat distCoeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        double reprojectionError;
        bool success;
    };
    
    bool calibrateCamera(const std::string& cornerDataFolder, const std::string& imageFolder,
                        const std::string& outputFolder, int boardWidth, int boardHeight,
                        float squareSize, int imageWidth, int imageHeight, 
                        bool showReprojection, const std::string& rectifiedOutputFolder);
    
    CalibrationResult calibrateCameraFromPoints(const std::vector<std::vector<cv::Point2f>>& imagePoints,
                                               const std::vector<std::vector<cv::Point3f>>& objectPoints,
                                               cv::Size imageSize);
    
    bool saveCalibrationData(const CalibrationResult& result, const std::string& outputFile);
    
    bool loadCalibrationData(const std::string& inputFile, CalibrationResult& result);
}