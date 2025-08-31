# MATLAB-OpenCV 矩阵和单位转换说明

## 问题概述
本文档说明了MATLAB双目标定参数导出到OpenCV格式时的矩阵组织和单位处理问题及其修正方案。

## 主要问题和修正

### 1. 内参矩阵格式转换

#### 问题描述
MATLAB和OpenCV对相机内参矩阵的存储格式不同：

**MATLAB内参矩阵格式：**
```
[fx  0   0 ]
[0   fy  0 ]
[cx  cy  1 ]
```

**OpenCV内参矩阵格式：**
```
[fx  0   cx]
[0   fy  cy]
[0   0   1 ]
```

#### 原始MATLAB代码问题
```matlab
% 错误的代码
fprintf(fid, '    %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f\n', ...
    stereoParams.CameraParameters1.IntrinsicMatrix(1,1), 0, stereoParams.CameraParameters1.IntrinsicMatrix(1,3), ...
    0, stereoParams.CameraParameters1.IntrinsicMatrix(2,2), stereoParams.CameraParameters1.IntrinsicMatrix(2,3), ...
    0, 0, 1);
```

问题：使用了 `IntrinsicMatrix(1,3)` 和 `IntrinsicMatrix(2,3)`，但在MATLAB的内参矩阵中，主点坐标实际存储在第3行。

#### 修正后的MATLAB代码
```matlab
% 正确的代码
fprintf(fid, '    %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f\n', ...
    stereoParams.CameraParameters1.IntrinsicMatrix(1,1), 0, stereoParams.CameraParameters1.IntrinsicMatrix(3,1), ...
    0, stereoParams.CameraParameters1.IntrinsicMatrix(2,2), stereoParams.CameraParameters1.IntrinsicMatrix(3,2), ...
    0, 0, 1);
```

#### 结果对比
**修正前（错误）：**
```xml
<data>
  1694.02784656 0.00000000 0.00000000 0.00000000 1705.22588901 0.00000000 0.00000000 0.00000000 1.00000000
</data>
```

**修正后（正确）：**
```xml
<data>
  1694.02780000 0.00000000 1641.83500000 0.00000000 1705.22590000 1228.39720000 0.00000000 0.00000000 1.00000000
</data>
```

### 2. 旋转矩阵转置问题

#### 问题描述
原始代码中使用了不必要的转置操作：
```matlab
fprintf(fid, '    %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f\n', stereoParams.RotationOfCamera2');
```

#### 修正方案
移除转置操作，直接使用原始矩阵：
```matlab
fprintf(fid, '    %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f %.8f\n', stereoParams.RotationOfCamera2);
```

**原因：** MATLAB和OpenCV都使用行优先存储，且旋转矩阵的坐标系约定一致，无需转置。

### 3. 单位一致性检查

#### 确认的单位
- **棋盘格尺寸：** 8.2毫米 (`squareSize = 8.200000e+00`)
- **平移向量：** 毫米单位 (数值范围：-253.159767, 76.250386, 122.919070)
- **焦距：** 像素单位 (FocalLength)
- **主点：** 像素单位 (PrincipalPoint)

#### 单位转换验证
所有参数单位保持一致，无需额外转换。

### 4. 畸变系数顺序

#### OpenCV畸变系数顺序
```
[k1, k2, p1, p2, k3]
```
其中：
- k1, k2, k3: 径向畸变系数
- p1, p2: 切向畸变系数

#### MATLAB参数映射
```matlab
% 正确的顺序
stereoParams.CameraParameters1.RadialDistortion(1),     % k1
stereoParams.CameraParameters1.RadialDistortion(2),     % k2
stereoParams.CameraParameters1.TangentialDistortion(1), % p1
stereoParams.CameraParameters1.TangentialDistortion(2), % p2
stereoParams.CameraParameters1.RadialDistortion(3)      % k3
```

## 修正效果验证

### 修正前的问题
1. 主点坐标丢失（cx=0, cy=0）
2. 可能的旋转矩阵方向错误
3. 缺乏详细的格式转换说明

### 修正后的改进
1. ✅ 正确的主点坐标 (cx=1641.835, cy=1228.3972 for Camera1)
2. ✅ 正确的旋转矩阵（无不必要转置）
3. ✅ 详细的转换说明和注释
4. ✅ 保持单位一致性

## 使用建议

1. **验证参数：** 在使用修正后的XML文件进行三维重建前，建议验证参数的合理性
2. **坐标系检查：** 确认旋转矩阵的符号和方向与实际相机配置一致
3. **单位检查：** 确认所有距离参数使用统一的毫米单位

## 相关文件
- `matlab代码.txt`: MATLAB标定代码（已修正）
- `MyProject/Calibration_Data/stereo_calibration.xml`: 修正后的OpenCV标定文件
- `MyProject/Calibration_Data/stereo_calibration_original.xml`: 原始文件备份
- `MyProject/Calibration_Data/camera_parameters.txt`: 人类可读的参数文件