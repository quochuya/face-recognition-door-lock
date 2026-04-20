#include "include/FaceTrainer.h"
#include "include/FaceDetector.h"
#include "include/DataCollector.h"
#include <iostream>
#include <string>

//Đường dẫn hằng số để tránh lỗi gõ tay và dễ bảo trì hơn
const std::string CASCADE_PATH = "../models/haarcascade_frontalface_default.xml";
const std::string DATASET_PATH = "../data/dataset";
const std::string TRAINER_PATH = "../models/trainer.yml";

int main() {
    int choice;
    std::cout << "\n--- HE THONG QUAN LY SMART LOCK ---\n";
    std::cout << "1. Them nguoi dung moi (Auto Chup Anh)\n";
    std::cout << "2. Huan luyen mo hinh AI (Train)\n";
    std::cout << "3. Chay Camera an ninh (Run)\n";
    std::cout << "Chon chuc nang (1, 2 hoac 3): ";
    std::cin >> choice;

    if (choice == 1) {
        int id;
        std::cout << "Nhap ID cho nguoi dung moi (VD: 1 cho Admin, 2 cho Khach): ";
        std::cin >> id;
        DataCollector collector;
        collector.collect(id, CASCADE_PATH, DATASET_PATH, 50);
    } 
    else if (choice == 2) {
        FaceTrainer trainer;
        trainer.trainModel(DATASET_PATH, TRAINER_PATH);
    } 
    else if (choice == 3) {
        FaceDetector detector;
        if (detector.loadModels(CASCADE_PATH, TRAINER_PATH)) {
            detector.startCamera();
        }
    } else {
        std::cout << "Lua chon khong hop le!" << std::endl;
    }

    return 0;
}