#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "Loaders/CSVLoader.h"
#include <functional>

class SpeedSensor {
public:
    SpeedSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback);
    ~SpeedSensor();

    void start();
    void stop();

private:
    void readDataLoop();

    CSVLoader csvLoader;
    std::vector<std::vector<std::string>> speedData;
    std::atomic<bool> running;
    std::thread sensorThread;
    std::function<void(double, double)> saveDataCallback;  // (time_stamp, speed)
};
