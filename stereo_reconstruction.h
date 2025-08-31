#pragma once
#include <opencv2/opencv.hpp>
#include <string>

namespace StereoReconstruction {
    struct ReconstructionParams {
        std::string leftImagePath;
        std::string rightImagePath;
        std::string outputFolder;
        std::string calibrationFile;
        int outputFormat; // 0=PLY, 1=OBJ
        int meshGeneration; // 0=None, 1=Delaunay, 2=Poisson
        int quality; // 1-5
        bool useColorTexture;
        float maxDepth;
        float minDepth;
        int algorithm; // 0=BM, 1=SGBM, 2=GC
        int postProcessing; // 0=None, 1=Median, 2=Bilateral
    };
    
    struct ReconstructionOutput {
        cv::Mat depthMap;
        cv::Mat residualMap;
        cv::Mat rectifiedLeft;
        cv::Mat rectifiedRight;
        cv::Mat pointCloud3D;
        bool success;
    };
    
    bool reconstruct3DModel(const std::string& leftImagePath, const std::string& rightImagePath,
                           const std::string& outputFolder, const std::string& calibrationFile,
                           int outputFormat, int meshGeneration, int quality, bool useColorTexture,
                           float maxDepth, float minDepth, int algorithm, int postProcessing);
    
    ReconstructionOutput performStereoReconstruction(const ReconstructionParams& params);
    
    cv::Mat computeDepthMap(const cv::Mat& rectifiedLeft, const cv::Mat& rectifiedRight, 
                           int algorithm, int quality);
    
    cv::Mat computeResidualMap(const cv::Mat& leftImage, const cv::Mat& rightImage,
                              const cv::Mat& depthMap);
    
    bool savePointCloud(const cv::Mat& points3D, const cv::Mat& colors, 
                       const std::string& filename, int format);
    
    bool saveDepthMap(const cv::Mat& depthMap, const std::string& filename);
    
    bool saveRectifiedImages(const cv::Mat& rectLeft, const cv::Mat& rectRight, 
                            const std::string& outputFolder);
}