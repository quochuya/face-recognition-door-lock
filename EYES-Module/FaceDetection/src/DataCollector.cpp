#include "DataCollector.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void DataCollector::collect(int userId, const std::string &cascadePath, const std::string &savePath, int numImages)
{
    // Tạo thư mục dataset nếu chưa tồn tại
    try
    {
        if (!fs::exists(savePath))
        {
            fs::create_directories(savePath);
            std::cout << "[INFO] Da tao thu muc: " << savePath << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Loi: Khong the tao thu muc dataset!" << e.what() << std::endl;
        return;
    }

    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(cascadePath))
    {
        std::cout << "Loi: Khong the load Haar Cascade de chup anh!" << std::endl;
        return;
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "Loi: Khong the mo Camera!" << std::endl;
        return;
    }

    cv::Mat frame, grayFrame;
    int count = 0;

    std::cout << "\n[HET SUC LUU Y] Nhin thang vao camera. He thong dang chup anh..." << std::endl;

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(grayFrame, faces, 1.2, 5, 0, cv::Size(100, 100));

        if (!faces.empty())
        {

            // Tim khuon mat lon nhat
            cv::Rect largestFace;
            int maxArea = 0;

            for (const auto &face : faces)
            {
                int area = face.width * face.height;

                if (area > maxArea)
                {
                    maxArea = area;
                    largestFace = face;
                }
            }

            // Cat khuon mat lon nhat
            cv::Mat faceROI = grayFrame(largestFace);

            // Can bang histogram de cai thien anh
            cv::equalizeHist(faceROI, faceROI);

            // Dua tat ca anh ve cung kich thuoc
            cv::resize(faceROI, faceROI, cv::Size(200, 200));

            count++;

            std::string filename = savePath + "/" +
                                   std::to_string(userId) + "." +
                                   std::to_string(count) + ".jpg";

            bool success = cv::imwrite(filename, faceROI);

            if (!success)
            {
                std::cout << "[WARNING] Khong the luu file: "
                          << filename << std::endl;
            }

            // Ve khung khuon mat lon nhat
            cv::rectangle(frame,
                          largestFace,
                          cv::Scalar(0, 255, 0),
                          2);

            cv::waitKey(100);
        }
    }

    cap.release();
    cv::destroyAllWindows();
    std::cout << "\n[THANH CONG] Da chup xong " << count << " buc anh cho ID " << userId << "!" << std::endl;
}