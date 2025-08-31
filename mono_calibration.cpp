#include "mono_calibration.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace MonoCalibration {

CalibrationResult calibrateCameraFromPoints(const std::vector<std::vector<cv::Point2f>>& imagePoints,
                                           const std::vector<std::vector<cv::Point3f>>& objectPoints,
                                           cv::Size imageSize) {
    CalibrationResult result;
    result.success = false;
    
    try {
        result.reprojectionError = cv::calibrateCamera(
            objectPoints, imagePoints, imageSize,
            result.cameraMatrix, result.distCoeffs,
            result.rvecs, result.tvecs
        );
        
        result.success = true;
        std::cout << "Monocular calibration RMS error: " << result.reprojectionError << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in camera calibration: " << e.what() << std::endl;
    }
    
    return result;
}

bool saveCalibrationData(const CalibrationResult& result, const std::string& outputFile) {
    cv::FileStorage fs(outputFile, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Cannot open file for writing: " << outputFile << std::endl;
        return false;
    }
    
    fs << "Camera_Matrix" << result.cameraMatrix;
    fs << "Distortion_Coefficients" << result.distCoeffs;
    fs << "Reprojection_Error" << result.reprojectionError;
    
    fs.release();
    return true;
}

bool loadCalibrationData(const std::string& inputFile, CalibrationResult& result) {
    cv::FileStorage fs(inputFile, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Cannot open file for reading: " << inputFile << std::endl;
        return false;
    }
    
    fs["Camera_Matrix"] >> result.cameraMatrix;
    fs["Distortion_Coefficients"] >> result.distCoeffs;
    fs["Reprojection_Error"] >> result.reprojectionError;
    
    result.success = true;
    fs.release();
    return true;
}

bool calibrateCamera(const std::string& cornerDataFolder, const std::string& imageFolder,
                    const std::string& outputFolder, int boardWidth, int boardHeight,
                    float squareSize, int imageWidth, int imageHeight, 
                    bool showReprojection, const std::string& rectifiedOutputFolder) {
    
    std::cout << "Monocular calibration functionality - using stereo calibration results" << std::endl;
    fs::create_directories(outputFolder);
    
    return true;
}

}