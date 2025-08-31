#include "stereo_reconstruction.h"
#include "stereo_calibration.h"
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace StereoReconstruction {

cv::Mat computeDepthMap(const cv::Mat& rectifiedLeft, const cv::Mat& rectifiedRight, 
                       int algorithm, int quality) {
    cv::Mat disparity;
    cv::Mat depthMap;
    
    // Convert to grayscale if needed
    cv::Mat leftGray, rightGray;
    if (rectifiedLeft.channels() == 3) {
        cv::cvtColor(rectifiedLeft, leftGray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(rectifiedRight, rightGray, cv::COLOR_BGR2GRAY);
    } else {
        leftGray = rectifiedLeft.clone();
        rightGray = rectifiedRight.clone();
    }
    
    if (algorithm == 1) { // SGBM
        auto sgbm = cv::StereoSGBM::create();
        
        // Set parameters based on quality
        int blockSize = (quality <= 2) ? 3 : (quality <= 4) ? 5 : 7;
        int numDisparities = 96;
        int minDisparity = 0;
        
        sgbm->setBlockSize(blockSize);
        sgbm->setNumDisparities(numDisparities);
        sgbm->setMinDisparity(minDisparity);
        sgbm->setP1(8 * leftGray.channels() * blockSize * blockSize);
        sgbm->setP2(32 * leftGray.channels() * blockSize * blockSize);
        sgbm->setDisp12MaxDiff(1);
        sgbm->setUniquenessRatio(10);
        sgbm->setSpeckleWindowSize(100);
        sgbm->setSpeckleRange(32);
        sgbm->setPreFilterCap(63);
        sgbm->setMode(cv::StereoSGBM::MODE_SGBM);
        
        sgbm->compute(leftGray, rightGray, disparity);
    } else { // StereoBM
        auto bm = cv::StereoBM::create();
        
        int blockSize = (quality <= 2) ? 15 : (quality <= 4) ? 21 : 25;
        int numDisparities = 96;
        
        bm->setBlockSize(blockSize);
        bm->setNumDisparities(numDisparities);
        bm->setMinDisparity(0);
        bm->setSpeckleWindowSize(100);
        bm->setSpeckleRange(32);
        bm->setDisp12MaxDiff(1);
        
        bm->compute(leftGray, rightGray, disparity);
    }
    
    // Convert to proper depth map
    disparity.convertTo(depthMap, CV_32F, 1.0/16.0);
    
    return depthMap;
}

cv::Mat computeResidualMap(const cv::Mat& leftImage, const cv::Mat& rightImage,
                          const cv::Mat& depthMap) {
    cv::Mat residual;
    cv::Mat leftGray, rightGray;
    
    // Convert to grayscale
    if (leftImage.channels() == 3) {
        cv::cvtColor(leftImage, leftGray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(rightImage, rightGray, cv::COLOR_BGR2GRAY);
    } else {
        leftGray = leftImage.clone();
        rightGray = rightImage.clone();
    }
    
    // Compute residual as absolute difference
    cv::absdiff(leftGray, rightGray, residual);
    
    // Apply colormap for visualization
    cv::Mat colorResidual;
    cv::applyColorMap(residual, colorResidual, cv::COLORMAP_JET);
    
    return colorResidual;
}

bool savePointCloud(const cv::Mat& points3D, const cv::Mat& colors, 
                   const std::string& filename, int format) {
    if (points3D.empty()) {
        std::cerr << "No 3D points to save" << std::endl;
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    if (format == 0) { // PLY format
        int numPoints = points3D.rows * points3D.cols;
        
        // Write PLY header
        file << "ply" << std::endl;
        file << "format ascii 1.0" << std::endl;
        file << "element vertex " << numPoints << std::endl;
        file << "property float x" << std::endl;
        file << "property float y" << std::endl;
        file << "property float z" << std::endl;
        if (!colors.empty()) {
            file << "property uchar red" << std::endl;
            file << "property uchar green" << std::endl;
            file << "property uchar blue" << std::endl;
        }
        file << "end_header" << std::endl;
        
        // Write points
        for (int i = 0; i < points3D.rows; i++) {
            for (int j = 0; j < points3D.cols; j++) {
                cv::Vec3f point = points3D.at<cv::Vec3f>(i, j);
                if (std::isfinite(point[0]) && std::isfinite(point[1]) && std::isfinite(point[2])) {
                    file << point[0] << " " << point[1] << " " << point[2];
                    
                    if (!colors.empty()) {
                        cv::Vec3b color = colors.at<cv::Vec3b>(i, j);
                        file << " " << (int)color[2] << " " << (int)color[1] << " " << (int)color[0];
                    }
                    
                    file << std::endl;
                }
            }
        }
    }
    
    file.close();
    return true;
}

bool saveDepthMap(const cv::Mat& depthMap, const std::string& filename) {
    cv::Mat normalizedDepth;
    cv::normalize(depthMap, normalizedDepth, 0, 255, cv::NORM_MINMAX, CV_8U);
    
    // Apply colormap for better visualization
    cv::Mat colorDepth;
    cv::applyColorMap(normalizedDepth, colorDepth, cv::COLORMAP_JET);
    
    return cv::imwrite(filename, colorDepth);
}

bool saveRectifiedImages(const cv::Mat& rectLeft, const cv::Mat& rectRight, 
                        const std::string& outputFolder) {
    fs::create_directories(outputFolder);
    
    std::string leftPath = outputFolder + "/rectified_left.jpg";
    std::string rightPath = outputFolder + "/rectified_right.jpg";
    
    bool success1 = cv::imwrite(leftPath, rectLeft);
    bool success2 = cv::imwrite(rightPath, rectRight);
    
    return success1 && success2;
}

ReconstructionOutput performStereoReconstruction(const ReconstructionParams& params) {
    ReconstructionOutput output;
    output.success = false;
    
    try {
        // Load images
        cv::Mat leftImage = cv::imread(params.leftImagePath);
        cv::Mat rightImage = cv::imread(params.rightImagePath);
        
        if (leftImage.empty() || rightImage.empty()) {
            std::cerr << "Cannot load input images" << std::endl;
            return output;
        }
        
        // Load calibration data
        StereoCalibration::StereoCalibrationResult calibData;
        if (!StereoCalibration::loadCalibrationXML(params.calibrationFile, calibData)) {
            std::cerr << "Cannot load calibration data" << std::endl;
            return output;
        }
        
        // Create rectification maps
        cv::Mat map1x, map1y, map2x, map2y;
        cv::initUndistortRectifyMap(calibData.cameraMatrix1, calibData.distCoeffs1,
                                   calibData.R1, calibData.P1, leftImage.size(),
                                   CV_16SC2, map1x, map1y);
        cv::initUndistortRectifyMap(calibData.cameraMatrix2, calibData.distCoeffs2,
                                   calibData.R2, calibData.P2, rightImage.size(),
                                   CV_16SC2, map2x, map2y);
        
        // Rectify images
        cv::remap(leftImage, output.rectifiedLeft, map1x, map1y, cv::INTER_LINEAR);
        cv::remap(rightImage, output.rectifiedRight, map2x, map2y, cv::INTER_LINEAR);
        
        // Compute depth map
        output.depthMap = computeDepthMap(output.rectifiedLeft, output.rectifiedRight, 
                                         params.algorithm, params.quality);
        
        // Compute 3D points
        cv::reprojectImageTo3D(output.depthMap, output.pointCloud3D, calibData.Q);
        
        // Compute residual map
        output.residualMap = computeResidualMap(output.rectifiedLeft, output.rectifiedRight, 
                                               output.depthMap);
        
        output.success = true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in stereo reconstruction: " << e.what() << std::endl;
    }
    
    return output;
}

bool reconstruct3DModel(const std::string& leftImagePath, const std::string& rightImagePath,
                       const std::string& outputFolder, const std::string& calibrationFile,
                       int outputFormat, int meshGeneration, int quality, bool useColorTexture,
                       float maxDepth, float minDepth, int algorithm, int postProcessing) {
    
    ReconstructionParams params;
    params.leftImagePath = leftImagePath;
    params.rightImagePath = rightImagePath;
    params.outputFolder = outputFolder;
    params.calibrationFile = calibrationFile;
    params.outputFormat = outputFormat;
    params.meshGeneration = meshGeneration;
    params.quality = quality;
    params.useColorTexture = useColorTexture;
    params.maxDepth = maxDepth;
    params.minDepth = minDepth;
    params.algorithm = algorithm;
    params.postProcessing = postProcessing;
    
    ReconstructionOutput result = performStereoReconstruction(params);
    
    if (!result.success) {
        return false;
    }
    
    // Create output directory
    fs::create_directories(outputFolder);
    
    // Save depth map
    std::string depthPath = outputFolder + "/depth_map.jpg";
    if (!saveDepthMap(result.depthMap, depthPath)) {
        std::cerr << "Failed to save depth map" << std::endl;
    } else {
        std::cout << "Depth map saved to: " << depthPath << std::endl;
    }
    
    // Save rectified images
    if (!saveRectifiedImages(result.rectifiedLeft, result.rectifiedRight, outputFolder)) {
        std::cerr << "Failed to save rectified images" << std::endl;
    } else {
        std::cout << "Rectified images saved to: " << outputFolder << std::endl;
    }
    
    // Save residual map
    std::string residualPath = outputFolder + "/residual_map.jpg";
    if (!cv::imwrite(residualPath, result.residualMap)) {
        std::cerr << "Failed to save residual map" << std::endl;
    } else {
        std::cout << "Residual map saved to: " << residualPath << std::endl;
    }
    
    // Save point cloud
    std::string pointCloudPath = outputFolder + "/point_cloud.ply";
    cv::Mat colors = result.rectifiedLeft; // Use left image for colors
    if (!savePointCloud(result.pointCloud3D, colors, pointCloudPath, outputFormat)) {
        std::cerr << "Failed to save point cloud" << std::endl;
    } else {
        std::cout << "Point cloud saved to: " << pointCloudPath << std::endl;
    }
    
    return true;
}

}