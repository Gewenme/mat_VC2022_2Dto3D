#include "image_resize.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "corner_detection.h"
#include "model_viewer.h"

#include <iostream>
#include <fstream>
#include <filesystem>

int main() {



  //   //Step 2: 角点检测与绘制
  //  bool success3 = detectAndDrawCorners(
  //      "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize/camL_resized",//左图像文件夹本地地址，已存在
  //      "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/left_jiaodian",//左图像文件夹输出地址，不存在会自动创建，存在则存入
  //      9,    // 棋盘内角点数
  //      6,    // 棋盘内角点数
		//1.0f  // 图像放大比例因子，以防图像太小检测不到角点
  //  );

  //  if (success3) {
  //      std::cout << "左相机角点检测完成!" << std::endl;
  //  }
  //  else {
  //      std::cerr << "左相机角点检测失败!" << std::endl;
  //      return -1;
  //  }

  //  bool success4 =detectAndDrawCorners(
  //      "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize/camR_resized",//同理
		//"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/right_jiaodian",  //同理
  //      9,    // 棋盘内角点数
  //      6,    // 棋盘内角点数
		//1.0f  // 图像放大比例因子，以防图像太小检测不到角点
  //  );

  //  if (success4) {
  //      std::cout << "右相机角点检测完成!" << std::endl;
  //  }
  //  else {
  //      std::cerr << "右相机角点检测失败!" << std::endl;
  //      return -1;
  //  }


 //    //Step 3: 单目标定
 //   bool success1 = MonoCalibration::calibrateCamera(
 //       "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/left_jiaodian/corner_data",//左图像文件夹本地地址，已存在
	//	"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize//camL_resized",//等待矫正的图像文件夹地址，用step1的图像
	//	"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step3_biaoding/left_calibration",//左图像标定参数文件输出地址，不存在会自动创建，存在则存入
 //       9,
 //       6,
 //       0.0082f,
 //       3264,
 //       2448,
 //       false,
 //       "  " // 单目标定输出校正后的图像地址，不存在会自动创建，存在则存入
 //   );

 //   if (success1) {
 //       std::cout << "左相机标定成功!" << std::endl;
 //       );
 //   } else {
 //       std::cerr << "左相机标定失败!" << std::endl;
 //       return -1;
 //   }
	////同理右相机标定
 //   
 //   //。。。。。。。



   //  双目标定
   // StereoCalibration::StereoCalibrationResult stereoResult =
   //     StereoCalibration::calibrateStereoCamera(
   //         "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/left_jiaodian/corner_data",//左图像文件夹本地地址，已存在
   //         "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/right_jiaodian/corner_data",//右图像文件夹本地地址，已存在
   //         "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step4_shuangmu_biaoding/stereo_calibration",//双目标定参数文件输出地址，不存在会自动创建，存在则存入
			//9, 6, 0.0082f,
			//true,  // 使用有理畸变模型
			//true   // 使用高精度模式
   //     );

   // if (stereoResult.success) {
   //     std::cout << "双目标定成功，重投影误差: " << stereoResult.reprojectionError << std::endl;
   // }
   // else {
   //     std::cerr << "双目标定失败" << std::endl;
   // }



 // Step 5: 三维重建
bool success = reconstruct3DModel(
    "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/picture_new/build_pic/left/6.jpg",
    "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/picture_new/build_pic/right/6.jpg",
    "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step5_3D_reconstruction/3D_reconstruction",
    "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step4_shuangmu_biaoding/stereo_calibration.xml", // XML格式的标定文件
    0, // PLY格式
    0, // 不生成网格
    3, // 中等质量
    true, // 使用颜色纹理
    10.0f, // 最大深度10米
    0.1f, // 最小深度0.1米
    1, // 使用SGBM算法
    2 // 使用双边滤波后处理
);

if (success) {
    std::cout << "三维重建成功!" << std::endl;
}
else {
    std::cerr << "三维重建失败!" << std::endl;
    return -1;
    //   //Step 6: 三维模型查看
    //   


    return 0;
}



























}