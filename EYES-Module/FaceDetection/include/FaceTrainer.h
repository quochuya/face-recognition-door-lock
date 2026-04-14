#ifndef FACETRAINER_H
#define FACETRAINER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include <string>

class FaceTrainer {
public:
    // Hàm nhận đường dẫn thư mục ảnh và đường dẫn lưu file model
    void trainModel(const std::string& datasetPath, const std::string& savePath);
};

#endif