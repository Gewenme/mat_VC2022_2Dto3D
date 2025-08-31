#pragma once
#include <opencv2/opencv.hpp>
#include <string>

namespace Modeling3D {
    struct ModelingParams {
        std::string leftImagePath;
        std::string rightImagePath;
        std::string calibrationFile;
        std::string outputFolder;
        int qualityLevel;        // 1-5
        bool generatePointCloud; // 是否生成点云
        bool generateDepthMap;   // 是否生成深度图
        bool generateResidualMap;// 是否生成残差图
        bool generateRectifiedImages; // 是否生成矫正图
    };
    
    struct ModelingResult {
        cv::Mat depthMap;
        cv::Mat residualMap;
        cv::Mat rectifiedLeft;
        cv::Mat rectifiedRight;
        cv::Mat pointCloud3D;
        cv::Mat colorImage;
        std::string pointCloudFile;
        bool success;
        double processingTime;
    };
    
    // 主要建模函数
    ModelingResult performModeling(const ModelingParams& params);
    
    // 专门针对 left/1.jpg 和 right/1.jpg 的建模函数
    bool modelSpecificImagePair(const std::string& outputFolder, int qualityLevel = 3);
    
    // 生成完整的三维模型
    bool generateComplete3DModel(const std::string& leftImage, const std::string& rightImage,
                                const std::string& calibrationFile, const std::string& outputFolder);
    
    // 对比矫正图与效果图
    bool compareWithReferenceImage(const std::string& rectifiedImage, 
                                  const std::string& referenceImage);
    
    // 显示处理结果统计
    void showProcessingStats(const ModelingResult& result);
}