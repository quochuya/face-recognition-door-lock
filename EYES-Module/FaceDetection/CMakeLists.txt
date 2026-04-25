cmake_minimum_required(VERSION 3.10)
project(SmartLock_Module1)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(SmartLock main.cpp ${SOURCES})

target_link_libraries(SmartLock ${OpenCV_LIBS} httplib::httplib nlohmann_json::nlohmann_json)