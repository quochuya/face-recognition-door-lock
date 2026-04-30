#include "FaceDetector.h"
#include <iostream>

FaceDetector::FaceDetector() {
    recognizer = cv::face::LBPHFaceRecognizer::create();
}

bool FaceDetector::loadModels(const std::string& cascadePath, const std::string& lbphPath) {
    if (!faceCascade.load(cascadePath)) {
        std::cout << "Loi: Khong the load Haar Cascade" << std::endl;
        return false;
    }
    try {
        recognizer->read(lbphPath);
    } catch (cv::Exception& e) {
        std::cout << "Loi: Khong the load file YML" << std::endl;
        return false;
    }
    std::cout << "Tai mo hinh thanh cong!" << std::endl;
    return true;
}

bool FaceDetector::detect(cv::Mat& frame, int& outId, double& outDistance) {
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(grayFrame, grayFrame); 

    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayFrame, faces, 1.2, 5, 0, cv::Size(80, 80));

    if (faces.empty()) {
        return false; 
    } 

    cv::Mat faceROI = grayFrame(faces[0]); 

    recognizer->predict(faceROI, outId, outDistance);

    cv::rectangle(frame, faces[0], cv::Scalar(0, 255, 0), 2);
    cv::putText(frame, "ID: " + std::to_string(outId) + " | " + std::to_string((int)outDistance), 
                cv::Point(faces[0].x, faces[0].y - 10), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

    return true; // Báo là đã tìm thấy và nhận diện xong
}