#include "corner_detection.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <iomanip>
#include <algorithm>

namespace CornerDetection {

bool detectChessboardCorners(const cv::Mat& image, int boardWidth, int boardHeight, 
                            std::vector<cv::Point2f>& corners) {
    cv::Size boardSize(boardWidth, boardHeight);
    
    // Convert to grayscale if necessary
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }
    
    // Detect corners
    bool found = cv::findChessboardCorners(gray, boardSize, corners,
        cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK);
    
    if (found) {
        // Refine corner positions
        cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
            cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
    }
    
    return found;
}

cv::Mat drawCornersWithNumbers(const cv::Mat& image, const std::vector<cv::Point2f>& corners, 
                              int boardWidth, int boardHeight) {
    cv::Mat result = image.clone();
    
    if (corners.empty()) {
        return result;
    }
    
    // Draw corners with + marks
    for (size_t i = 0; i < corners.size(); i++) {
        cv::Point2f pt = corners[i];
        
        // Draw red + mark
        cv::drawMarker(result, pt, cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 12, 2);
        
        // Draw corner number
        std::string text = std::to_string(i + 1);
        cv::Point textPos(pt.x + 15, pt.y - 5);
        cv::putText(result, text, textPos, cv::FONT_HERSHEY_SIMPLEX, 0.5, 
                   cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
    }
    
    // Draw special markers for the four corner points
    if (corners.size() >= boardWidth * boardHeight) {
        std::vector<int> cornerIndices = {0, boardWidth - 1, 
                                         (boardHeight - 1) * boardWidth, 
                                         boardHeight * boardWidth - 1};
        
        for (int idx : cornerIndices) {
            if (idx < corners.size()) {
                cv::circle(result, corners[idx], 16, cv::Scalar(255, 0, 0), 3);
            }
        }
    }
    
    return result;
}

bool detectAndDrawCorners(const std::string& inputFolder, const std::string& outputFolder,
                         int boardWidth, int boardHeight, float scaleFactor) {
    try {
        // Create output directory
        fs::create_directories(outputFolder);
        
        // Get all image files
        std::vector<std::string> imageFiles;
        for (const auto& entry : fs::directory_iterator(inputFolder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                    imageFiles.push_back(entry.path().string());
                }
            }
        }
        
        if (imageFiles.empty()) {
            std::cerr << "No image files found in " << inputFolder << std::endl;
            return false;
        }
        
        int successCount = 0;
        
        for (size_t i = 0; i < imageFiles.size(); i++) {
            cv::Mat image = cv::imread(imageFiles[i]);
            if (image.empty()) {
                std::cerr << "Cannot read image: " << imageFiles[i] << std::endl;
                continue;
            }
            
            // Resize if needed
            if (scaleFactor != 1.0f) {
                cv::resize(image, image, cv::Size(), scaleFactor, scaleFactor);
            }
            
            std::vector<cv::Point2f> corners;
            bool detected = detectChessboardCorners(image, boardWidth, boardHeight, corners);
            
            if (detected) {
                cv::Mat resultImage = drawCornersWithNumbers(image, corners, boardWidth, boardHeight);
                
                // Save result
                fs::path inputPath(imageFiles[i]);
                std::string outputFileName = "corner_detected_" + inputPath.filename().string();
                std::string outputPath = outputFolder + "/" + outputFileName;
                
                cv::imwrite(outputPath, resultImage);
                successCount++;
                
                std::cout << "Corner detection successful for: " << inputPath.filename() 
                         << " (" << corners.size() << " corners)" << std::endl;
            } else {
                std::cerr << "Corner detection failed for: " << imageFiles[i] << std::endl;
            }
        }
        
        std::cout << "Corner detection completed: " << successCount << "/" << imageFiles.size() 
                  << " images processed successfully" << std::endl;
        
        return successCount > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in corner detection: " << e.what() << std::endl;
        return false;
    }
}

bool saveCornerData(const std::vector<CornerData>& leftCorners, 
                   const std::vector<CornerData>& rightCorners,
                   const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Cannot open file for writing: " << outputFile << std::endl;
        return false;
    }
    
    file << "Image_Index, Camera, Point_Index, X, Y" << std::endl;
    
    // Save left camera corners
    for (size_t i = 0; i < leftCorners.size(); i++) {
        if (leftCorners[i].detected) {
            for (size_t j = 0; j < leftCorners[i].corners.size(); j++) {
                file << (i + 1) << ", left, " << (j + 1) << ", " 
                     << std::fixed << std::setprecision(4) 
                     << leftCorners[i].corners[j].x << ", " 
                     << leftCorners[i].corners[j].y << std::endl;
            }
        }
    }
    
    // Save right camera corners
    for (size_t i = 0; i < rightCorners.size(); i++) {
        if (rightCorners[i].detected) {
            for (size_t j = 0; j < rightCorners[i].corners.size(); j++) {
                file << (i + 1) << ", right, " << (j + 1) << ", " 
                     << std::fixed << std::setprecision(4) 
                     << rightCorners[i].corners[j].x << ", " 
                     << rightCorners[i].corners[j].y << std::endl;
            }
        }
    }
    
    file.close();
    return true;
}

}