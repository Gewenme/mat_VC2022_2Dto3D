#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace StereoCalibration {
    struct StereoCalibrationResult {
        cv::Mat cameraMatrix1, cameraMatrix2;
        cv::Mat distCoeffs1, distCoeffs2;
        cv::Mat R, T, E, F;
        cv::Mat R1, R2, P1, P2, Q;
        cv::Rect roi1, roi2;
        double reprojectionError;
        bool success;
    };
    
    StereoCalibrationResult calibrateStereoCamera(const std::string& leftCornerFolder,
                                                 const std::string& rightCornerFolder,
                                                 const std::string& outputFolder,
                                                 int boardWidth, int boardHeight,
                                                 float squareSize,
                                                 bool useRationalModel = true,
                                                 bool useHighPrecision = true);
    
    StereoCalibrationResult calibrateFromPoints(const std::vector<std::vector<cv::Point2f>>& leftPoints,
                                               const std::vector<std::vector<cv::Point2f>>& rightPoints,
                                               const std::vector<std::vector<cv::Point3f>>& objectPoints,
                                               cv::Size imageSize);
    
    bool saveCalibrationXML(const StereoCalibrationResult& result, const std::string& filename);
    
    bool loadCalibrationXML(const std::string& filename, StereoCalibrationResult& result);
    
    bool compareMatlabCalibration(const StereoCalibrationResult& cvResult, 
                                 const std::string& matlabParamsFile);
}