#include "include/FaceTrainer.h"
#include "include/FaceDetector.h"
#include <iostream>

int main() {
    int choice;
    std::cout << "--- HE THONG NHAN DIEN KHUON MAT ---\n";
    std::cout << "1. Huan luyen mo hinh (Train)\n";
    std::cout << "2. Chay Camera an ninh (Run)\n";
    std::cout << "Chon chuc nang (1 hoac 2): ";
    std::cin >> choice;

    if (choice == 1) {
        FaceTrainer trainer;
        trainer.trainModel("data/dataset", "models/trainer.yml");
    } 
    else if (choice == 2) {
        FaceDetector detector;
        if (detector.loadModels("models/haarcascade_frontalface_default.xml", "models/trainer.yml")) {
            detector.startCamera();
        }
    } else {
        std::cout << "Lua chon khong hop le!" << std::endl;
    }

    return 0;
}