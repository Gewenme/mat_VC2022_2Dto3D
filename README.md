# mat_VC2022_2Dto3D

## 项目概述
双目视觉3D重建项目，基于MATLAB标定数据进行高精度三维重建。

## 已完成功能

### Step 1: MATLAB标定数据读取
- ✅ 读取matlab.txt文件中的相机内外参数
- ✅ 理解参数格式、矩阵转置和单位
- ✅ 成功加载MyProject/Calibration_Data/中的标定数据

### Step 2: 三维重建C++程序
- ✅ 基于MATLAB导出的camera_parameters.txt和stereo_calibration.xml创建C++程序
- ✅ 支持picture/build_pic/left/1.jpg和picture/build_pic/right/1.jpg的三维重建
- ✅ 输出深度图、点云模型、残差图、矫正图（类似MATLAB双目标定矫正图）

### 新增功能
- ✅ **角点检测重新标定**: 放弃导出的角点检测图片，重新进行角点检测
- ✅ **角点编号显示**: 在角点检测图上显示角点顺序号码
- ✅ **参数对比验证**: 对比OpenCV标定结果与MATLAB参数，确保差别在合理范围内
- ✅ **完整建模管道**: 生成所有所需输出文件

## 输出结果

### 角点检测结果
- 成功检测54个角点（9x6棋盘格）
- 角点图像带有序号标注
- 保存在output/left_corners/和output/right_corners/

### 标定参数对比
OpenCV结果与MATLAB参数完全一致：
- 相机1焦距: [1694.0278, 1705.2259] 
- 相机2焦距: [2565.9883, 2565.1437]
- 旋转矩阵和平移向量匹配

### 三维重建输出
- `depth_map.jpg`: 彩色深度图
- `residual_map.jpg`: 残差图
- `rectified_left.jpg`, `rectified_right.jpg`: 矫正图
- `point_cloud.ply`: 点云模型（235,008个有效3D点）

## 编译和运行

```bash
# 安装依赖
sudo apt install libopencv-dev cmake build-essential

# 编译
mkdir build && cd build
cmake .. && make

# 运行主程序
./build/bin/stereo_vision

# 运行建模示例
./build/bin/modeling_example
```

## 文件结构
- 所有头文件(.h)和源文件(.cpp)都在main.cpp同目录下
- 建模部分独立模块: modeling_3d.h, modeling_3d.cpp
- 主函数调用示例: main_modeling_example.cpp