#include "include/FaceDetector.h"
#include "include/ApiClient.h"
#include "include/DataCollector.h"
#include "include/FaceTrainer.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <httplib.h>
#include <filesystem>
#include <set>
#include <mutex> // Thư viện dùng để khóa luồng

const std::string CASCADE_PATH = "../models/haarcascade_frontalface_default.xml";
const std::string TRAINER_PATH = "../models/trainer.yml";
const std::string DATASET_PATH = "../data/dataset";

// --- CÁC BIẾN TOÀN CỤC CHO GIAO DIỆN ---
cv::Mat globalFrame;
std::mutex frameMutex;
bool isCamActive = false;
std::string activeCamName = "Camera";

void runCameraWorker(int camIndex, const std::string &camName, ApiClient &api)
{
    FaceDetector detector;
    // Load não AI
    if (!detector.loadModels(CASCADE_PATH, TRAINER_PATH))
    {
        return;
    }

    cv::VideoCapture cap(camIndex);
    if (!cap.isOpened())
        return;

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

        // BƯỚC 1: COPY ẢNH RA BIẾN TOÀN CỤC ĐỂ UI VẼ MÀ KHÔNG BỊ TREO LUỒNG
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            frame.copyTo(globalFrame);
            isCamActive = true;
            activeCamName = camName;
        }

        int recognizedId = -1;
        double distance = 999.0;

        // Gọi FaceDetector ra khám bức ảnh
        bool isFaceDetected = detector.detect(frame, recognizedId, distance);

        if (isFaceDetected)
        {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUnlockTime).count();

            if (isFirstTime || elapsedTime >= 3) 
            {
                // BƯỚC 2: CHỐT CHẶN BÓNG MA - CHỈ MỞ KHI SAI SỐ < 65.0
                if (distance < 65.0) {
                    std::cout << "[" << camName << "] Nhan dien ID: " << recognizedId 
                              << " | Sai so: " << distance << " -> Dang goi API mo cua!" << std::endl;
                    
                    api.sendUnlockRequest(camName, recognizedId, distance);
                } else {
                    std::cout << "[WARNING] Thay mat nhung sai so qua cao (" << distance << "), tu choi mo cua!" << std::endl;
                }

                lastUnlockTime = currentTime; 
                isFirstTime = false;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}

int main()
{
    ApiClient api("127.0.0.1", 8080);

    httplib::Server svr;
    std::cout << "[SYSTEM] EYES-MODULE KHOI DONG\n";

    svr.Post("/add_cam", [&api](const httplib::Request &req, httplib::Response &res)
             {
        try {
            auto j = nlohmann::json::parse(req.body);
            int camIndex = j["cam_index"];
            std::string camName = j["cam_name"];

            std::cout << "\n[COMMAND] Nhan lenh tu Java: Bat camera " << camName << " (ID phan cung: " << camIndex << ")\n";

            // TẠO LUỒNG MỚI VÀ THẢ TRÔI
            std::thread(runCameraWorker, camIndex, camName, std::ref(api)).detach();

            nlohmann::json responseJson;
            responseJson["status"] = "success";
            responseJson["message"] = "Da bat luong camera " + camName;
            res.set_content(responseJson.dump(), "application/json");

        } catch (const std::exception& e) {
            std::cout << "[ERROR] Loi khi phan tich lenh tu Java: " << e.what() << "\n";
            res.status = 400;
            res.set_content("{\"status\": \"error\"}", "application/json");
        } });

    // --- ENDPOINT: CHỤP ẢNH CHO NGƯỜI DÙNG ---
    svr.Post("/collect_face_data", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            auto j = nlohmann::json::parse(req.body);
            int userId = j["user_id"];
            int numImages = j.value("num_images", 50);

            std::cout << "\n[COMMAND] BẮT ĐẦU THU THẬP DỮ LIỆU cho User ID: " << userId << "\n";

            DataCollector collector;
            collector.collect(userId, CASCADE_PATH, DATASET_PATH, numImages);

            nlohmann::json responseJson;
            responseJson["status"] = "success";
            responseJson["message"] = "Da chup xong " + std::to_string(numImages) + " buc anh cho User " + std::to_string(userId);
            responseJson["user_id"] = userId;
            res.set_content(responseJson.dump(), "application/json");

        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"" + std::string(e.what()) + "\"}", "application/json");
        } });

    // --- ENDPOINT: HUẤN LUYỆN MÔ HÌNH ---
    svr.Post("/train_model", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            std::cout << "\n[COMMAND] BẮT ĐẦU HUẤN LUYỆN MÔ HÌNH\n";
            
            FaceTrainer trainer;
            trainer.trainModel(DATASET_PATH, TRAINER_PATH);

            nlohmann::json responseJson;
            responseJson["status"] = "success";
            responseJson["message"] = "Da huan luyen xong mo hinh!";
            res.set_content(responseJson.dump(), "application/json");

        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"" + std::string(e.what()) + "\"}", "application/json");
        } });

    // --- ENDPOINT: KIỂM TRA DATASET ---
    svr.Get("/dataset_info", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            namespace fs = std::filesystem;
            int fileCount = 0;
            std::set<int> userIds;

            for (const auto& entry : fs::directory_iterator(DATASET_PATH)) {
                if (fs::is_regular_file(entry)) {
                    fileCount++;
                    std::string filename = entry.path().filename().string();
                    try {
                        int userId = std::stoi(filename.substr(0, filename.find('.')));
                        userIds.insert(userId);
                    } catch (...) {}
                }
            }

            nlohmann::json responseJson;
            responseJson["status"] = "success";
            responseJson["total_images"] = fileCount;
            responseJson["unique_users"] = (int)userIds.size();
            responseJson["user_ids"] = nlohmann::json::array();
            for (int id : userIds) {
                responseJson["user_ids"].push_back(id);
            }
            res.set_content(responseJson.dump(), "application/json");

        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content("{\"status\": \"error\"}", "application/json");
        } });

    std::cout << "[SYSTEM] Dang cho lenh mo Camera tu Java tai cong 8081...\n";
    std::cout << "[INFO] Dataset path: " << DATASET_PATH << "\n";
    std::cout << "[INFO] Trainer path: " << TRAINER_PATH << "\n";
    
    // BƯỚC 3: ĐẨY SERVER LẮNG NGHE XUỐNG LUỒNG NGẦM ĐỂ TRÁNH TREO MÁY
    std::thread([&svr]() {
        svr.listen("0.0.0.0", 8081);
    }).detach();

    // BƯỚC 4: LUỒNG CHÍNH DÙNG ĐỂ VẼ GIAO DIỆN CAMERA
    while (true) {
        cv::Mat displayFrame;
        std::string winName;

        // Lấy khung hình mới nhất một cách an toàn
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            if (isCamActive && !globalFrame.empty()) {
                globalFrame.copyTo(displayFrame);
                winName = activeCamName;
            }
        }

        if (!displayFrame.empty()) {
            cv::imshow(winName, displayFrame);
        } else {
            // Hiển thị màn hình chờ cực ngầu nếu chưa có lệnh bật Camera
            cv::Mat standby = cv::Mat::zeros(480, 640, CV_8UC3);
            cv::putText(standby, "WAITING FOR JAVA SIGNAL...", cv::Point(80, 240), 
                        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
            cv::imshow("SmartLock Dashboard", standby);
        }

        // Nhấn phím ESC để thoát toàn bộ chương trình
        if (cv::waitKey(30) == 27) break; 
    }

    return 0;
}