cmake_minimum_required(VERSION 3.21)

# Название проекта
project(signature_detector)

# Укажите стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Пути к исходным файлам
file(GLOB SRC_FILES "main.cpp")

# Пути к заголовочным файлам
include_directories(
    /usr/include/opencv4
)

# Пути к библиотекам
link_directories(
    /usr/lib
)

# Ссылки на библиотеки
set(LINK_LIBS
    opencv_core
    opencv_imgcodecs
    opencv_videoio
    opencv_imgproc
    opencv_highgui

)
# Исполняемый файл
add_executable(main ${SRC_FILES})

# Линковка библиотек
target_link_libraries(main ${LINK_LIBS})
