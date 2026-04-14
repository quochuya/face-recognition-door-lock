cmake_minimum_required(VERSION 3.10)
project(SmartLock_Module1)

set(CMAKE_CXX_STANDARD 17)

# Tìm OpenCV trên máy
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Gom tất cả file trong src
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(SmartLock main.cpp ${SOURCES})
target_link_libraries(SmartLock ${OpenCV_LIBS})