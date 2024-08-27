#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "Loaders/CSVLoader.h"
#include <functional>

class GPSSensor {
public:
    GPSSensor(const std::string& csvFilePath, std::function<void(double, double, double, double)> saveDataCallback);
    ~GPSSensor();

    void start();
    void stop();

private:
    void readDataLoop();

    CSVLoader csvLoader;
    std::vector<std::vector<std::string>> gpsData;
    std::atomic<bool> running;
    std::thread sensorThread;
    std::function<void(double, double, double, double)> saveDataCallback;  // (time_stamp, latitude, longitude, height)
};