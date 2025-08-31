cmake_minimum_required(VERSION 3.10)
project(StereoVision3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable
add_executable(stereo_vision 
    main.cpp
    corner_detection.cpp
    stereo_calibration.cpp
    stereo_reconstruction.cpp
    mono_calibration.cpp
    image_resize.cpp
    model_viewer.cpp
)

# Link OpenCV libraries
target_link_libraries(stereo_vision ${OpenCV_LIBS} stdc++fs)

# Set output directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)