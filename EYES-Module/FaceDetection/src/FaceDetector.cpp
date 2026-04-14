#include "FaceDetector.h"
#include <iostream>

FaceDetector::FaceDetector() {
    // Khởi tạo thuật toán LBPH
    recognizer = cv::face::LBPHFaceRecognizer::create();
}

bool FaceDetector::loadModels(const std::string& cascadePath, const std::string& lbphPath) {
    if (!faceCascade.load(cascadePath)) {
        std::cout << "Loi: Khong the load Haar Cascade tu " << cascadePath << std::endl;
        return false;
    }
    try {
        recognizer->read(lbphPath);
    } catch (cv::Exception& e) {
        std::cout << "Loi: Khong the load file YML tu " << lbphPath << std::endl;
        return false;
    }
    std::cout << "Tai mo hinh thanh cong!" << std::endl;
    return true;
}

void FaceDetector::startCamera() {
    cv::VideoCapture cap(0); // 0 là camera mặc định
    if (!cap.isOpened()) {
        std::cout << "Loi: Khong the mo Camera!" << std::endl;
        return;
    }

    cv::Mat frame, grayFrame;
    std::cout << "He thong dang chay. Nhan ESC de thoat..." << std::endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // 1. Tiền xử lý (Giống hệt lúc huấn luyện)
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(grayFrame, grayFrame);

        // 2. Tìm khuôn mặt
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(grayFrame, faces, 1.2, 5, 0, cv::Size(100, 100));

        // 3. Xử lý từng khuôn mặt tìm được
        for (const auto& face : faces) {
            cv::Mat faceROI = grayFrame(face); // Cắt lấy vùng mặt

            int id = -1;
            double distance = 0.0;
            // 4. Nhận diện!
            recognizer->predict(faceROI, id, distance);

            // 5. Vẽ khung và hiển thị kết quả
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            
            std::string label = "Unknown";
            cv::Scalar color = cv::Scalar(0, 0, 255); // Màu đỏ cho người lạ

            // Distance (khoảng cách) càng nhỏ càng giống. Ngưỡng LBPH thường < 70 là an toàn
            if (distance < 70.0) { 
                if (id == 1) {
                    label = "Admin (ID: 1)";
                    color = cv::Scalar(0, 255, 0); // Màu xanh cho Admin
                    
                    // TODO: CHỖ NÀY SẼ GỌI API SANG SPRING BOOT (BRAIN_Module)
                    // ApiClient::sendUnlockRequest(id);
                }
            }

            // Ghi chữ lên màn hình
            cv::putText(frame, label + " (Sai so: " + std::to_string((int)distance) + ")", 
                        cv::Point(face.x, face.y - 10), 
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, color, 2);
        }

        cv::imshow("EYES Module - Security Camera", frame);
        if (cv::waitKey(30) == 27) break; // Nhấn phím ESC để thoát
    }
}