#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>

class FaceDetector {
private:
    cv::CascadeClassifier faceCascade;
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer;

public:
    FaceDetector();
    bool loadModels(const std::string& cascadePath, const std::string& lbphPath);
    
    bool detect(cv::Mat& frame, int& outId, double& outDistance); 
};