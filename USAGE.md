# 双目视觉3D重建项目使用说明

## 编译和运行

### 1. 编译项目
```bash
mkdir build
cd build
cmake ..
make
```

### 2. 运行程序
```bash
./build/bin/stereo_vision
```

## 功能说明

### 主要功能
1. **角点检测与编号显示**: 检测棋盘格角点并在图像上显示编号
2. **双目标定**: 使用MATLAB标定参数进行双目系统标定
3. **三维重建**: 生成深度图、点云、残差图和矫正图

### 输出文件
- `output/left_corners/`: 左相机角点检测结果（带编号）
- `output/right_corners/`: 右相机角点检测结果（带编号）
- `output/calibration/`: OpenCV格式的标定参数
- `output/reconstruction/`: 三维重建结果
  - `depth_map.jpg`: 深度图
  - `residual_map.jpg`: 残差图
  - `rectified_left.jpg`, `rectified_right.jpg`: 矫正图
  - `point_cloud.ply`: 点云模型

### 参数对比
程序会自动对比OpenCV标定结果与MATLAB标定结果，确保参数一致性。

## 代码结构

### 头文件
- `corner_detection.h`: 角点检测功能
- `stereo_calibration.h`: 双目标定功能
- `stereo_reconstruction.h`: 三维重建功能
- `mono_calibration.h`: 单目标定功能
- `image_resize.h`: 图像缩放功能
- `model_viewer.h`: 模型查看功能

### 源文件
对应的`.cpp`文件实现了各个头文件声明的功能。

### 主函数
`main.cpp` 包含了完整的处理流程示例。