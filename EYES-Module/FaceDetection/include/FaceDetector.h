#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>

class FaceDetector {
private:
    cv::CascadeClassifier faceCascade;                 // Dùng để tìm mặt (Haar)
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer;  // Dùng để nhận diện (LBPH)

public:
    FaceDetector();
    // Hàm nạp 2 file mô hình từ thư mục models
    bool loadModels(const std::string& cascadePath, const std::string& lbphPath);
    // Hàm mở camera và chạy nhận diện liên tục
    void startCamera();
};