#include "FaceTrainer.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void FaceTrainer::trainModel(const std::string& datasetPath, const std::string& savePath) {
    std::vector<cv::Mat> images;
    std::vector<int> labels;

    std::cout << "[INFO] Dang doc du lieu tu: " << datasetPath << " ..." << std::endl;

    // Kiểm tra thư mục tồn tại
    if (!fs::exists(datasetPath)) {
        std::cout << "[ERROR] Thu muc dataset khong ton tai: " << datasetPath << std::endl;
        return;
    }

    int imageCount = 0;
    for (const auto& entry : fs::directory_iterator(datasetPath)) {
        if (!fs::is_regular_file(entry)) continue;
        
        std::string path = entry.path().string();
        std::string filename = entry.path().filename().string();

        // 1. Đọc ảnh và chuyển thẳng sang ảnh xám (Grayscale)
        cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
        if (img.empty()) {
            std::cout << "[WARNING] Khong the doc anh: " << filename << std::endl;
            continue;
        }

        cv::equalizeHist(img, img);

        try {
            int id = std::stoi(filename.substr(0, filename.find('.')));
            images.push_back(img);
            labels.push_back(id);
            imageCount++;
        } catch (const std::exception& e) {
            std::cout << "[WARNING] Loi khi phan tich ten file: " << filename << std::endl;
            continue;
        }
    }

    if (images.empty()) {
        std::cout << "[ERROR] Khong tim thay anh nao de huan luyen! Vui long chup anh truoc." << std::endl;
        return;
    }

    std::cout << "[INFO] Tim thay " << imageCount << " buc anh tu " << labels.size() << " nhan" << std::endl;
    std::cout << "[INFO] Dang huan luyen mo hinh LBPH..." << std::endl;
    
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    model->train(images, labels);
    
    try {
        model->save(savePath);
        std::cout << "[SUCCESS] Mo hinh da duoc huan luyen va luu tai: " << savePath << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[ERROR] Khong the luu mo hinh: " << e.what() << std::endl;
    }
}