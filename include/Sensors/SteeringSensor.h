#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "Loaders/CSVLoader.h"
#include <functional>

class SteeringSensor {
public:
    SteeringSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback);
    ~SteeringSensor();

    void start();
    void stop();

private:
    void readDataLoop();

    CSVLoader csvLoader;
    std::vector<std::vector<std::string>> steeringData;
    std::atomic<bool> running;
    std::thread sensorThread;
    std::function<void(double, double)> saveDataCallback;  // (time_stamp, steering_angle)
};