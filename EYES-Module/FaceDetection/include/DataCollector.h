#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class DataCollector {
public:
    // Hàm nhận ID người dùng và số lượng ảnh cần chụp (mặc định 50)
    void collect(int userId, const std::string& cascadePath, const std::string& savePath, int numImages = 50);
};