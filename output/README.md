# 运行结果输出目录

此目录用于保存3D重建程序的所有运行结果。

## 目录结构

- `3D_reconstruction/`: 三维重建结果
  - 点云模型文件 (.ply)
  - 深度图
  - 残差图
  - 其他3D重建输出文件

- `corner_detection/`: 角点检测结果
  - `left/`: 左相机角点检测结果
  - `right/`: 右相机角点检测结果

- `calibration/`: 单目标定结果
  - `left/`: 左相机标定参数和校正图像
  - `right/`: 右相机标定参数和校正图像

- `stereo_calibration/`: 双目标定结果
  - 双目标定参数文件
  - 立体校正图像

## 输入数据位置

- 输入图像: `picture/build_pic/left/` 和 `picture/build_pic/right/`
- 标定数据: `MyProject/Calibration_Data/`