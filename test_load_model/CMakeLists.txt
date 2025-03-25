cmake_minimum_required(VERSION 3.10)

project(test_load)

find_package(cppflow REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(test_load main.cpp)


target_include_directories(
    test_load PUBLIC
    cppflow::cppflow
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
    test_load PUBLIC
    cppflow::cppflow
    opencv_core
    opencv_imgcodecs
    opencv_videoio
    opencv_imgproc
    opencv_highgui
)
