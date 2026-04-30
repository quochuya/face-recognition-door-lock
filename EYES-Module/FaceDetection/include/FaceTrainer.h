#ifndef FACETRAINER_H
#define FACETRAINER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include <string>

class FaceTrainer {
public:
    void trainModel(const std::string& datasetPath, const std::string& savePath);
};

#endif