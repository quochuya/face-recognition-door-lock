
#include "include/FaceDetector.h"
#include "include/ApiClient.h"
#include "include/DataCollector.h"
#include "include/FaceTrainer.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <httplib.h>
#include <filesystem>
#include <set>
#include <mutex>


// CÁC BIẾN TOÀN CỤC (QUẢN LÝ LUỒNG & TRẠNG THÁI)

std::atomic<bool> isCollectingMode{false};
std::atomic<int> targetUserId{-1};
std::atomic<int> currentImageCount{0};
const int MAX_IMAGES = 50;

// Các cờ tín hiệu cho luồng Kiểm tra mặt 
std::atomic<bool> requestCheckFace{false};
std::atomic<bool> checkFaceReady{false};
std::atomic<bool> hasFaceInFrame{false};
std::atomic<int> checkedId{-1};
std::atomic<double> checkedDistance{999.0};

// Cờ báo hiệu cần nạp lại Model AI sau khi Web ra lệnh Train
std::atomic<bool> needReloadModel{false};

const std::string CASCADE_PATH = "../models/haarcascade_frontalface_default.xml";
const std::string TRAINER_PATH = "../models/trainer.yml";
const std::string DATASET_PATH = "../data/dataset";


std::atomic<bool> keepCameraRunning[4]{false, false, false, false};
cv::Mat globalFrames[4];
bool isCamActive[4] = {false, false, false, false};
std::string activeCamNames[4] = {"", "", "", ""};
std::mutex frameMutex;


void runCameraWorker(int camIndex, const std::string &camName, int slotIndex, ApiClient &api)
{
    std::cout << "[SYSTEM] Khoi chay luong Camera: " << camName << " (Index: " << camIndex << ") tai o chu nhat so: " << slotIndex + 1 << "\n";
   
    FaceDetector detector;
    if (!detector.loadModels(CASCADE_PATH, TRAINER_PATH)) {
        std::cout << "[ERROR] Slot " << slotIndex + 1 << " khong the nap duoc Model AI!\n";
        return;
    }

    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(CASCADE_PATH)) {
        std::cout << "[ERROR] Khong the load Haar Cascade o camera slot " << slotIndex + 1 << "!\n";
        return;
    }

    
    cv::VideoCapture cap;
    bool connected = false;

    // 1. Thử mở bằng cổng chính xác do Web/Java truyền xuống trước
    std::cout << "[SYSTEM] Slot " << slotIndex + 1 << " dang thu mo Hardware Index mac dinh: " << camIndex << "...\n";
    cap.open(camIndex, cv::CAP_DSHOW);
    if (cap.isOpened()) {
        connected = true;
    } else {
        // 2. Nếu thất bại (Lỗi kẹt driver DroidCam), tự động kích hoạt chế độ "Quét cổng phần cứng"
        std::cout << "[WARNING] Index " << camIndex << " bi loi. Dang tu dong do tim cong Camera kha dung...\n";
        for (int testIndex = 0; testIndex < 6; testIndex++) {
            // Không thử lại cổng vừa lỗi để tiết kiệm thời gian
            if (testIndex == camIndex) continue; 
            
            cap.open(testIndex, cv::CAP_DSHOW);
            if (cap.isOpened()) {
                // Kiểm tra nhanh xem frame có lấy được không, tránh camera ảo rỗng
                cv::Mat testFrame;
                cap >> testFrame;
                if (!testFrame.empty()) {
                    std::cout << "[SUCCESS] Da tim thay thiet bi thay the phu hop! Hardware Index thuc te: " << testIndex << " cho Slot " << slotIndex + 1 << "\n";
                    connected = true;
                    break;
                }
                cap.release(); 
            }
        }
    }


    if (!connected) {
        std::cout << "[ERROR] Slot " << slotIndex + 1 << " THAT BAI: Khong tim thay bat ky Camera/DroidCam nao dang active tren may!\n";
        return;
    }


    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1); // Giữ buffer cực thấp để lấy frame thời gian thực, tránh trễ hình

    cv::Mat frame;
    auto lastUnlockTime = std::chrono::steady_clock::now();
    bool isFirstTime = true;
    
    keepCameraRunning[slotIndex] = true;

    while (keepCameraRunning[slotIndex])
    {
        cap >> frame;
        if (frame.empty()) {
            // Nếu mất frame, ngủ 10ms rồi thử lại chứ không được treo luồng
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Cập nhật frame sạch vào mảng dùng chung cho UI hiển thị
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            frame.copyTo(globalFrames[slotIndex]);
            isCamActive[slotIndex] = true;
            activeCamNames[slotIndex] = camName;
        }

        // Nạp lại Model AI nếu nhận được tín hiệu thay đổi từ API Train
        if (needReloadModel) {
            detector.loadModels(CASCADE_PATH, TRAINER_PATH);
        }

        if (isCollectingMode)
        {
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            std::vector<cv::Rect> faces;
            faceCascade.detectMultiScale(gray, faces, 1.1, 4);

            if (!faces.empty()) {
                cv::Rect faceBox = faces[0];
                cv::Mat faceROI = gray(faceBox);
                cv::resize(faceROI, faceROI, cv::Size(200, 200));

                std::string filepath = DATASET_PATH + "/" + std::to_string(targetUserId) + "." + std::to_string(currentImageCount + 1) + ".jpg";
                cv::imwrite(filepath, faceROI);

                currentImageCount++;
                std::cout << "[DATA] Camera " << camName << " chup: " << currentImageCount << "/" << MAX_IMAGES << "\r";

                if (currentImageCount >= MAX_IMAGES) {
                    isCollectingMode = false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        else
        {
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            std::vector<cv::Rect> faces;
            faceCascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(30, 30));
            bool physicalFaceFound = !faces.empty();

            int recognizedId = -1;
            double distance = 999.0;
            const double THRESHOLD = 65.0;

            bool isFaceDetected = detector.detect(frame, recognizedId, distance);

            if (requestCheckFace && physicalFaceFound) {
                hasFaceInFrame = true;
                checkedId = (isFaceDetected && distance < THRESHOLD) ? recognizedId : -1;
                checkedDistance = distance;
                checkFaceReady = true;
                requestCheckFace = false;
            }

            if (isFaceDetected && !requestCheckFace) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUnlockTime).count();

                if (isFirstTime || elapsedTime >= 4) {
                    if (distance < THRESHOLD) {
                        std::cout << "[" << camName << "] OK - ID: " << recognizedId << " (Dist: " << distance << ") -> Mo cua!" << std::endl;
                        api.sendUnlockRequest(camName, recognizedId, distance);
                    } else {
                        std::cout << "[" << camName << " - WARNING] Phat hien sai so cao (" << distance << "), tu choi!" << std::endl;
                    }
                    lastUnlockTime = currentTime;
                    isFirstTime = false;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    {
        std::lock_guard<std::mutex> lock(frameMutex);
        isCamActive[slotIndex] = false;
        globalFrames[slotIndex].release();
    }
    cap.release();
    std::cout << "[SYSTEM] Luong Camera " << camName << " o Slot " << slotIndex + 1 << " da dung han.\n";
}

int main()
{
    ApiClient api("127.0.0.1", 8080);
    httplib::Server svr;
    std::cout << "[SYSTEM] COLOURED MULTI-CAMERA AGENT CORES READY\n";

    svr.Post("/add_cam", [&api](const httplib::Request &req, httplib::Response &res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            int camIndex = j["cam_index"].get<int>();
            std::string camName = j["cam_name"].get<std::string>();
            
            int slotIndex = 0;
            if (j.contains("slot_index")) {
                slotIndex = j["slot_index"].get<int>();
            } else {
                for (int i = 0; i < 4; i++) {
                    if (!isCamActive[i]) {
                        slotIndex = i;
                        break;
                    }
                }
            }

            if (slotIndex < 0 || slotIndex >= 4) slotIndex = 0;

            if (keepCameraRunning[slotIndex]) {
                keepCameraRunning[slotIndex] = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
            }


            std::thread(runCameraWorker, camIndex, camName, slotIndex, std::ref(api)).detach();
            
            res.set_content("{\"status\": \"success\"}", "application/json");
        } catch (...) { 
            res.status = 400; 
            res.set_content("{\"status\": \"error\", \"message\": \"Loi parse JSON\"}", "application/json");
        }
    });


    svr.Post("/stop_cam", [](const httplib::Request &req, httplib::Response &res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            if (j.contains("slot_index")) {
                int slotIndex = j["slot_index"].get<int>();
                if (slotIndex >= 0 && slotIndex < 4) keepCameraRunning[slotIndex] = false;
            } else {
                for (int i = 0; i < 4; i++) keepCameraRunning[i] = false;
            }
            res.set_content("{\"status\": \"success\"}", "application/json");
        } catch (...) {
            for (int i = 0; i < 4; i++) keepCameraRunning[i] = false;
            res.set_content("{\"status\": \"success\"}", "application/json");
        }
    });

    svr.Get("/check_face", [](const httplib::Request &req, httplib::Response &res) {
        hasFaceInFrame = false;  
        checkFaceReady = false;
        checkedId = -1;          
        checkedDistance = 999.0; 
        requestCheckFace = true; 

        int timeout = 20;
        while (!checkFaceReady && timeout > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            timeout--;
        }
        requestCheckFace = false; 

        nlohmann::json responseJson;
        responseJson["status"] = "success";
        responseJson["has_face"] = hasFaceInFrame.load(); 
        responseJson["id"] = checkedId.load();
        responseJson["distance"] = checkedDistance.load();
        res.set_content(responseJson.dump(), "application/json"); 
    });

    svr.Post("/collect_face_data", [](const httplib::Request &req, httplib::Response &res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            targetUserId = j["user_id"];
            currentImageCount = 0;
            isCollectingMode = true; 

            int timeout = 400; 
            while (isCollectingMode && timeout > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                timeout--;
            }

            nlohmann::json responseJson;
            if (timeout > 0) {
                responseJson["status"] = "success";
                responseJson["message"] = "Chup thanh cong 50 anh!";
            } else {
                responseJson["status"] = "error";
                responseJson["message"] = "Timeout!";
                isCollectingMode = false; 
            }
            res.set_content(responseJson.dump(), "application/json");
        } catch (...) { res.status = 400; } 
    });

    svr.Post("/train_model", [](const httplib::Request &req, httplib::Response &res) {
        try {
            FaceTrainer trainer;
            trainer.trainModel(DATASET_PATH, TRAINER_PATH);
            needReloadModel = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            needReloadModel = false; // Hạ cờ sau khi các luồng đã nhận lệnh xong

            nlohmann::json responseJson;
            responseJson["status"] = "success";
            res.set_content(responseJson.dump(), "application/json");
        } catch (...) { res.status = 400; } 
    });

    svr.Post("/delete_face_data", [](const httplib::Request &req, httplib::Response &res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            int userIdToDelete = j["user_id"];
            namespace fs = std::filesystem;
            int deletedCount = 0;

            for (const auto& entry : fs::directory_iterator(DATASET_PATH)) {
                if (fs::is_regular_file(entry)) {
                    std::string filename = entry.path().filename().string();
                    if (filename.rfind(std::to_string(userIdToDelete) + ".", 0) == 0) { 
                        fs::remove(entry);
                        deletedCount++;
                    }
                }
            }

            FaceTrainer trainer;
            trainer.trainModel(DATASET_PATH, TRAINER_PATH);
            needReloadModel = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            needReloadModel = false;

            nlohmann::json responseJson;
            responseJson["status"] = "success";
            res.set_content(responseJson.dump(), "application/json");
        } catch (...) { res.status = 500; } 
    });

    std::cout << "[SYSTEM] Dang lang nghe Java Core goi tai cong 8081...\n";
    std::thread([&svr]() { svr.listen("0.0.0.0", 8081); }).detach();


    const int FRAME_W = 640; 
    const int FRAME_H = 480;
    
    cv::namedWindow("SmartLock Security Matrix", cv::WINDOW_NORMAL);
    cv::setWindowProperty("SmartLock Security Matrix", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    cv::Rect rois[4] = {
        cv::Rect(0, 0, FRAME_W, FRAME_H),              // Ô 1: Trái Trên (Cửa chính)
        cv::Rect(FRAME_W, 0, FRAME_W, FRAME_H),        // Ô 2: Phải Trên (Cửa sau)
        cv::Rect(0, FRAME_H, FRAME_W, FRAME_H),        // Ô 3: Trái Dưới
        cv::Rect(FRAME_W, FRAME_H, FRAME_W, FRAME_H)   // Ô 4: Phải Dưới
    };

    while (true)
    {
        cv::Mat gridDisplay = cv::Mat::zeros(FRAME_H * 2, FRAME_W * 2, CV_8UC3);

        for (int i = 0; i < 4; i++)
        {
            cv::Mat displayFrame;
            std::string winName;
            bool isActive = false;

            {
                std::lock_guard<std::mutex> lock(frameMutex);
                if (isCamActive[i] && !globalFrames[i].empty())
                {
                    globalFrames[i].copyTo(displayFrame);
                    winName = activeCamNames[i];
                    isActive = true;
                }
            }

            if (isActive)
            {
                cv::resize(displayFrame, displayFrame, cv::Size(FRAME_W, FRAME_H));
                cv::putText(displayFrame, winName, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
                displayFrame.copyTo(gridDisplay(rois[i]));
            }
            else
            {
                cv::Mat standby = cv::Mat::zeros(FRAME_H, FRAME_W, CV_8UC3);
                std::string text = (i == 0) ? "MAIN DOOR: WAITING..." : 
                                   (i == 1) ? "BACK DOOR: NO SIGNAL" : 
                                   "SLOT " + std::to_string(i+1) + ": NO SIGNAL";
                
                int baseline = 0;
                cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.6, 2, &baseline);
                int textX = (FRAME_W - textSize.width) / 2;
                int textY = (FRAME_H + textSize.height) / 2;
                
                cv::putText(standby, text, cv::Point(textX, textY), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(100, 100, 100), 2);
                standby.copyTo(gridDisplay(rois[i]));
            }
        }

        // Kẻ vạch lưới phân chia
        cv::line(gridDisplay, cv::Point(FRAME_W, 0), cv::Point(FRAME_W, FRAME_H * 2), cv::Scalar(150, 150, 150), 2);
        cv::line(gridDisplay, cv::Point(0, FRAME_H), cv::Point(FRAME_W * 2, FRAME_H), cv::Scalar(150, 150, 150), 2);

        cv::imshow("SmartLock Security Matrix", gridDisplay);
        if (cv::waitKey(30) == 27) 
            break;
    }

    return 0;
}