// main_modeling_example.cpp - 建模部分主函数调用示例
#include "modeling_3d.h"
#include <iostream>

int main() {
    std::cout << "=== 三维建模主函数调用示例 ===" << std::endl;
    
    // 方法1: 使用专门的图像对建模函数
    std::cout << "\n方法1: 专门建模函数" << std::endl;
    bool success1 = Modeling3D::modelSpecificImagePair(
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/modeling_example1",
        4  // 高质量等级
    );
    
    // 方法2: 使用通用建模函数
    std::cout << "\n方法2: 通用建模函数" << std::endl;
    Modeling3D::ModelingParams params;
    params.leftImagePath = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/left/1.jpg";
    params.rightImagePath = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/right/1.jpg";
    params.calibrationFile = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/MyProject/Calibration_Data/stereo_calibration.xml";
    params.outputFolder = "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/modeling_example2";
    params.qualityLevel = 3;
    params.generatePointCloud = true;
    params.generateDepthMap = true;
    params.generateResidualMap = true;
    params.generateRectifiedImages = true;
    
    Modeling3D::ModelingResult result = Modeling3D::performModeling(params);
    
    // 方法3: 快速完整建模
    std::cout << "\n方法3: 快速完整建模" << std::endl;
    bool success3 = Modeling3D::generateComplete3DModel(
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/left/1.jpg",
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/picture/build_pic/right/1.jpg",
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/MyProject/Calibration_Data/stereo_calibration.xml",
        "/home/runner/work/mat_VC2022_2Dto3D/mat_VC2022_2Dto3D/output/modeling_example3"
    );
    
    if (success1 && result.success && success3) {
        std::cout << "\n=== 所有建模示例执行成功! ===" << std::endl;
        std::cout << "输出文件夹:" << std::endl;
        std::cout << "- modeling_example1/ (专门建模)" << std::endl;
        std::cout << "- modeling_example2/ (通用建模)" << std::endl;  
        std::cout << "- modeling_example3/ (快速建模)" << std::endl;
        return 0;
    } else {
        std::cerr << "建模示例执行失败!" << std::endl;
        return -1;
    }
}