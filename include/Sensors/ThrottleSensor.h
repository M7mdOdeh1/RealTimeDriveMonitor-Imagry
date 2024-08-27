#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "Loaders/CSVLoader.h"
#include <functional>

class ThrottleSensor {
public:
    ThrottleSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback);
    ~ThrottleSensor();

    void start();
    void stop();

private:
    void readDataLoop();

    CSVLoader csvLoader;
    std::vector<std::vector<std::string>> throttleData;
    std::atomic<bool> running;
    std::thread sensorThread;
    std::function<void(double, double)> saveDataCallback;  // (time_stamp, throttle_value)
};
