# mat\_VC2022\_2Dto3D

step1，读取matlab.txt文件，这是matlab导出相关数据的代码。看相机内外参数是如何保存的,格式是什么，矩阵是否转置，单位是否合适

step2, 根据根据matlab导出的文件MyProject\\Calibration\_Data\\camera\_parameters.txt和MyProject\\Calibration\_Data\\stereo\_calibration.xml读取参数以及理解其意义后，创建一个三维重建c++程序，输入为picture\\build\_pic\\left\\6.jpg和picture\\build\_pic\\right\\1.jpg，保存文件夹地址，质量参数（1-5级），输出深度图，点云模型，残差图，矫正图（就像matlab双目标定矫正图一样）。如“效果图.jpg”那样


## 运行结果保存位置

程序运行结果将保存在 `output/` 目录下：
- 三维重建结果: `output/3D_reconstruction/`
- 角点检测结果: `output/corner_detection/`
- 标定结果: `output/calibration/` 和 `output/stereo_calibration/`
