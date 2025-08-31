#include "modeling_3d.h"
#include "stereo_reconstruction.h"
#include "stereo_calibration.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <iomanip>
namespace fs = std::filesystem;

namespace Modeling3D {

ModelingResult performModeling(const ModelingParams& params) {
    ModelingResult result;
    result.success = false;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        std::cout << "开始三维建模..." << std::endl;
        std::cout << "左图像: " << params.leftImagePath << std::endl;
        std::cout << "右图像: " << params.rightImagePath << std::endl;
        std::cout << "质量等级: " << params.qualityLevel << std::endl;
        
        // 使用现有的重建功能
        StereoReconstruction::ReconstructionParams reconParams;
        reconParams.leftImagePath = params.leftImagePath;
        reconParams.rightImagePath = params.rightImagePath;
        reconParams.outputFolder = params.outputFolder;
        reconParams.calibrationFile = params.calibrationFile;
        reconParams.quality = params.qualityLevel;
        reconParams.algorithm = 1; // SGBM
        reconParams.useColorTexture = true;
        reconParams.maxDepth = 10.0f;
        reconParams.minDepth = 0.1f;
        reconParams.outputFormat = 0; // PLY
        reconParams.postProcessing = 2; // 双边滤波
        
        StereoReconstruction::ReconstructionOutput reconResult = 
            StereoReconstruction::performStereoReconstruction(reconParams);
        
        if (!reconResult.success) {
            std::cerr << "三维重建失败!" << std::endl;
            return result;
        }
        
        // 复制结果
        result.depthMap = reconResult.depthMap.clone();
        result.residualMap = reconResult.residualMap.clone();
        result.rectifiedLeft = reconResult.rectifiedLeft.clone();
        result.rectifiedRight = reconResult.rectifiedRight.clone();
        result.pointCloud3D = reconResult.pointCloud3D.clone();
        result.colorImage = reconResult.rectifiedLeft.clone();
        
        // 保存文件
        if (params.generateDepthMap) {
            std::string depthPath = params.outputFolder + "/depth_map.jpg";
            StereoReconstruction::saveDepthMap(result.depthMap, depthPath);
            std::cout << "深度图已保存: " << depthPath << std::endl;
        }
        
        if (params.generateRectifiedImages) {
            StereoReconstruction::saveRectifiedImages(result.rectifiedLeft, result.rectifiedRight, 
                                                     params.outputFolder);
            std::cout << "矫正图已保存" << std::endl;
        }
        
        if (params.generateResidualMap) {
            std::string residualPath = params.outputFolder + "/residual_map.jpg";
            cv::imwrite(residualPath, result.residualMap);
            std::cout << "残差图已保存: " << residualPath << std::endl;
        }
        
        if (params.generatePointCloud) {
            result.pointCloudFile = params.outputFolder + "/point_cloud.ply";
            StereoReconstruction::savePointCloud(result.pointCloud3D, result.colorImage, 
                                                result.pointCloudFile, 0);
            std::cout << "点云模型已保存: " << result.pointCloudFile << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.processingTime = duration.count() / 1000.0;
        
        result.success = true;
        
    } catch (const std::exception& e) {
        std::cerr << "建模过程中出错: " << e.what() << std::endl;
    }
    
    return result;
}

bool modelSpecificImagePair(const std::string& outputFolder, int qualityLevel) {
    std::cout << "\n=== 专门为 left/1.jpg 和 right/1.jpg 建模 ===" << std::endl;
    
    ModelingParams params;
    params.leftImagePath = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/left/1.jpg";
    params.rightImagePath = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/right/1.jpg";
    params.calibrationFile = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/MyProject/Calibration_Data/stereo_calibration.xml";
    params.outputFolder = outputFolder;
    params.qualityLevel = qualityLevel;
    params.generatePointCloud = true;
    params.generateDepthMap = true;
    params.generateResidualMap = true;
    params.generateRectifiedImages = true;
    
    ModelingResult result = performModeling(params);
    
    if (result.success) {
        showProcessingStats(result);
        
        // 对比矫正图与效果图
        std::string rectifiedPath = outputFolder + "/rectified_left.jpg";
        std::string referencePath = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/效果图.jpg";
        compareWithReferenceImage(rectifiedPath, referencePath);
    }
    
    return result.success;
}

bool generateComplete3DModel(const std::string& leftImage, const std::string& rightImage,
                            const std::string& calibrationFile, const std::string& outputFolder) {
    fs::create_directories(outputFolder);
    
    ModelingParams params;
    params.leftImagePath = leftImage;
    params.rightImagePath = rightImage;
    params.calibrationFile = calibrationFile;
    params.outputFolder = outputFolder;
    params.qualityLevel = 4; // 高质量
    params.generatePointCloud = true;
    params.generateDepthMap = true;
    params.generateResidualMap = true;
    params.generateRectifiedImages = true;
    
    ModelingResult result = performModeling(params);
    return result.success;
}

bool compareWithReferenceImage(const std::string& rectifiedImage, 
                              const std::string& referenceImage) {
    cv::Mat rectified = cv::imread(rectifiedImage);
    cv::Mat reference = cv::imread(referenceImage);
    
    if (rectified.empty()) {
        std::cerr << "无法读取矫正图: " << rectifiedImage << std::endl;
        return false;
    }
    
    if (reference.empty()) {
        std::cout << "参考图像不存在: " << referenceImage << std::endl;
        std::cout << "矫正图已生成，请手动对比质量" << std::endl;
        return true;
    }
    
    // 确保图像尺寸一致后再进行对比
    if (rectified.size() != reference.size()) {
        cv::resize(reference, reference, rectified.size());
    }
    
    // 确保图像通道数一致
    if (rectified.channels() != reference.channels()) {
        if (rectified.channels() == 3 && reference.channels() == 1) {
            cv::cvtColor(reference, reference, cv::COLOR_GRAY2BGR);
        } else if (rectified.channels() == 1 && reference.channels() == 3) {
            cv::cvtColor(reference, reference, cv::COLOR_BGR2GRAY);
        }
    }
    
    try {
        // 简单的图像质量对比
        cv::Mat diff;
        cv::absdiff(rectified, reference, diff);
        cv::Scalar meanDiff = cv::mean(diff);
        
        std::cout << "\n=== 矫正图质量对比 ===" << std::endl;
        std::cout << "与效果图的平均差异: " << meanDiff[0] << std::endl;
        
        if (meanDiff[0] < 50) {
            std::cout << "矫正图质量良好，与效果图相似" << std::endl;
        } else {
            std::cout << "矫正图与效果图存在较大差异，可能需要调整参数" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "图像对比过程中出错，跳过对比: " << e.what() << std::endl;
    }
    
    return true;
}

void showProcessingStats(const ModelingResult& result) {
    std::cout << "\n=== 处理结果统计 ===" << std::endl;
    std::cout << "处理时间: " << std::fixed << std::setprecision(2) << result.processingTime << " 秒" << std::endl;
    
    if (!result.depthMap.empty()) {
        double minVal, maxVal;
        cv::minMaxLoc(result.depthMap, &minVal, &maxVal);
        std::cout << "深度范围: " << minVal << " - " << maxVal << std::endl;
    }
    
    if (!result.pointCloud3D.empty()) {
        int validPoints = 0;
        for (int i = 0; i < result.pointCloud3D.rows; i++) {
            for (int j = 0; j < result.pointCloud3D.cols; j++) {
                cv::Vec3f point = result.pointCloud3D.at<cv::Vec3f>(i, j);
                if (std::isfinite(point[0]) && std::isfinite(point[1]) && std::isfinite(point[2])) {
                    validPoints++;
                }
            }
        }
        std::cout << "有效3D点数: " << validPoints << " / " 
                  << (result.pointCloud3D.rows * result.pointCloud3D.cols) << std::endl;
    }
    
    std::cout << "矫正图尺寸: " << result.rectifiedLeft.size() << std::endl;
}

}