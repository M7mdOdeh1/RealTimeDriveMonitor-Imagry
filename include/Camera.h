#pragma once

#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "include/Loaders/ImageLoader.h"
#include <string>
#include <functional>

class Camera {
public:
    Camera(const std::string& imageDir, std::function<void(double, const cv::Mat&)> saveFrameCallback);
    ~Camera();

    void start(double frameTime);
    void stop();

private:
    void captureLoop(double frameTime);

    ImageLoader imageLoader;
    std::vector<std::string> imageFiles;
    std::atomic<bool> running;
    std::thread cameraThread;
    std::function<void(double, const cv::Mat&)> saveFrameCallback;  // (time_stamp, frame)
};
