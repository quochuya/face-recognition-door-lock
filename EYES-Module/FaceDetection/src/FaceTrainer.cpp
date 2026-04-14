#include "FaceTrainer.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void FaceTrainer::trainModel(const std::string& datasetPath, const std::string& savePath) {
    std::vector<cv::Mat> images;
    std::vector<int> labels;

    std::cout << "Dang doc du lieu tu: " << datasetPath << " ..." << std::endl;

    for (const auto& entry : fs::directory_iterator(datasetPath)) {
        std::string path = entry.path().string();
        std::string filename = entry.path().filename().string();

        // 1. Đọc ảnh và chuyển thẳng sang ảnh xám (Grayscale) [cite: 85, 86]
        cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
        if (img.empty()) continue;

        // 2. Cân bằng ánh sáng (Histogram Equalization) 
        cv::equalizeHist(img, img);

        // 3. Cắt lấy ID từ tên file (Ví dụ: "1.15.jpg" -> ID = 1)
        int id = std::stoi(filename.substr(0, filename.find('.')));

        images.push_back(img);
        labels.push_back(id);
    }

    if (images.empty()) {
        std::cout << "Khong tim thay anh nao de huan luyen!" << std::endl;
        return;
    }

    std::cout << "Dang huan luyen mo hinh LBPH..." << std::endl;
    
    // 4. Khởi tạo và Train thuật toán LBPH
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    model->train(images, labels);
    
    // 5. Lưu kết quả ra file yml
    model->save(savePath);
    std::cout << "Thanh cong! Mo hinh da duoc luu tai: " << savePath << std::endl;
}