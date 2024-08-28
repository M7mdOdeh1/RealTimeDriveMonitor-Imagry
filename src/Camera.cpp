#include "../include/Camera.h"
#include <opencv2/opencv.hpp>
#include <chrono>
#include <iostream>

Camera::Camera(const std::string cameraName, const std::string& imageDir, std::function<void(double, const cv::Mat&)> saveFrameCallback)
    : cameraName(cameraName), imageLoader(imageDir), running(false), saveFrameCallback(saveFrameCallback)
{
    imageFiles = imageLoader.loadImages();
    
}

Camera::~Camera() {
    stop();
}

void Camera::start(double frameTime) {
    running = true;
    cameraThread = std::thread(&Camera::captureLoop, this, frameTime);
}

void Camera::stop() {
    running = false;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }
}

void Camera::captureLoop(double frameTime) {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= imageFiles.size()) {
            currentIndex = 0;  // if we've reached the end
        }


        std::string imagePath = imageLoader.getDirectory() + "/" + imageFiles[currentIndex];
        cv::Mat image = cv::imread(imagePath);


        if (!image.empty()) {
            
            // time_stamp from the file name
            std::string fileName = imageFiles[currentIndex];
            std::string timeStampStr = fileName.substr(0, fileName.find_last_of('.'));
            double time_stamp = std::stod(timeStampStr);

            // put the camera name into the image
            cv::putText(image, cameraName, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 3);

            // put the current index into the bottom left of the image
            cv::putText(image, "Frame Index: " + std::to_string(currentIndex), cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 3);

            // put the time_stamp into the image
            cv::putText(image, "Timestamp: " + std::to_string(time_stamp), cv::Point(10, 105), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 3);

            saveFrameCallback(time_stamp, image);  // Save the frame to DataManager with time_stamp
        }

        currentIndex++;

        // Sleep for the duration of the frameTime
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(frameTime)));
    }
}
