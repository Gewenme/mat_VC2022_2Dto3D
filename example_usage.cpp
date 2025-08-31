// example_usage.cpp - 使用示例代码
// 演示如何调用各个模块进行三维重建

#include "corner_detection.h"
#include "stereo_calibration.h" 
#include "stereo_reconstruction.h"
#include <iostream>

int main() {
    std::cout << "=== 三维重建模块使用示例 ===" << std::endl;
    
    // 示例1: 角点检测
    std::cout << "\n1. 角点检测示例:" << std::endl;
    bool cornerSuccess = CornerDetection::detectAndDrawCorners(
        "./input_images/left",     // 输入图像文件夹
        "./output/corners_left",   // 输出文件夹
        9, 6,                      // 棋盘格尺寸 (9x6)
        1.0f                       // 缩放因子
    );
    
    // 示例2: 双目标定
    std::cout << "\n2. 双目标定示例:" << std::endl;
    StereoCalibration::StereoCalibrationResult calibResult = 
        StereoCalibration::calibrateStereoCamera(
            "./corner_data/left",      // 左相机角点数据
            "./corner_data/right",     // 右相机角点数据
            "./output/calibration",    // 输出文件夹
            9, 6,                      // 棋盘格尺寸
            8.2f,                      // 方格大小(mm)
            true,                      // 使用有理畸变模型
            true                       // 高精度模式
        );
    
    // 示例3: 三维重建
    std::cout << "\n3. 三维重建示例:" << std::endl;
    bool reconSuccess = StereoReconstruction::reconstruct3DModel(
        "./images/left.jpg",           // 左图像
        "./images/right.jpg",          // 右图像
        "./output/3d_model",           // 输出文件夹
        "./calibration.xml",           // 标定文件
        0,                             // PLY格式
        0,                             // 不生成网格
        3,                             // 质量等级(1-5)
        true,                          // 使用彩色纹理
        10.0f,                         // 最大深度(m)
        0.1f,                          // 最小深度(m)
        1,                             // SGBM算法
        2                              // 双边滤波后处理
    );
    
    if (cornerSuccess && calibResult.success && reconSuccess) {
        std::cout << "\n所有处理步骤完成成功!" << std::endl;
        return 0;
    } else {
        std::cerr << "\n处理过程中出现错误!" << std::endl;
        return -1;
    }
}