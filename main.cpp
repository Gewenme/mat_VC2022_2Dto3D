#include "image_resize.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "corner_detection.h"
#include "model_viewer.h"

#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

int main() {
    std::cout << "=== 双目视觉3D重建项目 ===" << std::endl;
    
    // Step 1: Corner detection with numbered corners display
    std::cout << "\n--- Step 1: 角点检测与编号显示 ---" << std::endl;
    
    // First, check if we have point_pic directory, if not use build_pic
    std::string leftInputFolder = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/point_pic/left";
    std::string rightInputFolder = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/point_pic/right";
    
    // If point_pic doesn't exist, use build_pic for corner detection
    if (!fs::exists(leftInputFolder)) {
        leftInputFolder = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/left";
        rightInputFolder = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/right";
    }
    
    // Perform corner detection on calibration images
    bool leftSuccess = CornerDetection::detectAndDrawCorners(
        leftInputFolder,
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/left_corners",
        9, 6, 1.0f
    );
    
    bool rightSuccess = CornerDetection::detectAndDrawCorners(
        rightInputFolder, 
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/right_corners",
        9, 6, 1.0f
    );
    
    if (!leftSuccess || !rightSuccess) {
        std::cerr << "角点检测失败!" << std::endl;
        return -1;
    }
    
    std::cout << "角点检测完成!" << std::endl;
    
    // Step 2: Stereo calibration and comparison with MATLAB
    std::cout << "\n--- Step 2: 双目标定与MATLAB对比 ---" << std::endl;
    
    StereoCalibration::StereoCalibrationResult stereoResult = 
        StereoCalibration::calibrateStereoCamera(
            "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/left_corners",
            "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/right_corners",
            "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/calibration",
            9, 6, 8.2f, true, true
        );
    
    if (stereoResult.success) {
        std::cout << "双目标定成功，重投影误差: " << stereoResult.reprojectionError << std::endl;
        
        // Compare with MATLAB results
        StereoCalibration::compareMatlabCalibration(stereoResult, 
            "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/MyProject/Calibration_Data/camera_parameters.txt");
    } else {
        std::cerr << "双目标定失败!" << std::endl;
        return -1;
    }
    
    // Step 3: 3D reconstruction for specific image pair
    std::cout << "\n--- Step 3: 三维重建 (left/1.jpg 和 right/1.jpg) ---" << std::endl;
    
    bool reconstructionSuccess = StereoReconstruction::reconstruct3DModel(
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/left/1.jpg",
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/right/1.jpg",
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/reconstruction",
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/MyProject/Calibration_Data/stereo_calibration.xml",
        0, // PLY格式
        0, // 不生成网格
        3, // 中等质量
        true, // 使用颜色纹理
        10.0f, // 最大深度10米
        0.1f, // 最小深度0.1米
        1, // 使用SGBM算法
        2 // 使用双边滤波后处理
    );
    
    if (reconstructionSuccess) {
        std::cout << "三维重建成功!" << std::endl;
        std::cout << "输出文件保存在: /home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/reconstruction" << std::endl;
        std::cout << "- 深度图: depth_map.jpg" << std::endl;
        std::cout << "- 残差图: residual_map.jpg" << std::endl;
        std::cout << "- 矫正图: rectified_left.jpg, rectified_right.jpg" << std::endl;
        std::cout << "- 点云模型: point_cloud.ply" << std::endl;
    } else {
        std::cerr << "三维重建失败!" << std::endl;
        return -1;
    }
    
    std::cout << "\n=== 所有处理完成! ===" << std::endl;
    return 0;
}