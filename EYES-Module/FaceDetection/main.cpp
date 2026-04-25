#include "include/FaceDetector.h"
#include "include/ApiClient.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono> 

const std::string CASCADE_PATH = "../models/haarcascade_frontalface_default.xml";
const std::string TRAINER_PATH = "../models/trainer.yml";

void runCameraWorker(int camIndex, const std::string &camName, ApiClient &api)
{
    FaceDetector detector;
    // Load não AI
    if (!detector.loadModels(CASCADE_PATH, TRAINER_PATH))
    {
        return;
    }

    cv::VideoCapture cap(camIndex);
    if (!cap.isOpened()) return;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

    cv::Mat frame;

    auto lastUnlockTime = std::chrono::steady_clock::now();
    bool isFirstTime = true; 

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            continue;
        bool faceRecognized = true;
        int recognizedId = 1;
        double distance = 45.0;

        if (faceRecognized)
        {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUnlockTime).count();

            if (isFirstTime || elapsedTime >= 3) {
                std::cout << "[" << camName << "] Nhan dien ID: " << recognizedId << " -> Goi API!" << std::endl;
                std::thread([&api, camName, recognizedId, distance]() {
                    api.sendUnlockRequest(camName, recognizedId, distance);
                }).detach();

                lastUnlockTime = currentTime; 
                isFirstTime = false;
            }
        }

        cv::imshow(camName, frame);
        

        if (cv::waitKey(30) == 27)
            break;
    }
}

int main()
{
    ApiClient api("127.0.0.1", 8080);

    std::cout << "[SYSTEM] Dang khoi dong cac luong Camera..." << std::endl;

    std::thread t1(runCameraWorker, 0, "CUA_CHINH", std::ref(api));
    t1.join();

    return 0;
}