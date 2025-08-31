#include "image_resize.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace ImageResize {

cv::Mat resizeImage(const cv::Mat& image, float scaleFactor, int targetWidth, int targetHeight) {
    cv::Mat resized;
    
    if (targetWidth > 0 && targetHeight > 0) {
        cv::resize(image, resized, cv::Size(targetWidth, targetHeight));
    } else if (scaleFactor != 1.0f) {
        cv::resize(image, resized, cv::Size(), scaleFactor, scaleFactor);
    } else {
        resized = image.clone();
    }
    
    return resized;
}

bool resizeImages(const std::string& inputFolder, const std::string& outputFolder, 
                 float scaleFactor, int targetWidth, int targetHeight) {
    try {
        fs::create_directories(outputFolder);
        
        int processedCount = 0;
        
        for (const auto& entry : fs::directory_iterator(inputFolder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                    cv::Mat image = cv::imread(entry.path().string());
                    if (!image.empty()) {
                        cv::Mat resized = resizeImage(image, scaleFactor, targetWidth, targetHeight);
                        
                        std::string outputPath = outputFolder + "/" + entry.path().filename().string();
                        if (cv::imwrite(outputPath, resized)) {
                            processedCount++;
                            std::cout << "Resized: " << entry.path().filename() << std::endl;
                        }
                    }
                }
            }
        }
        
        std::cout << "Image resize completed: " << processedCount << " images processed" << std::endl;
        return processedCount > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in image resize: " << e.what() << std::endl;
        return false;
    }
}

}