#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include "Loaders/CSVLoader.h"
#include <functional>

class IMUSensor {
public:
    IMUSensor(const std::string& csvFilePath, std::function<void(double, double, double, double, double, double, double, double, double, double)> saveDataCallback);
    ~IMUSensor();

    void start();
    void stop();

private:
    void readDataLoop();

    CSVLoader csvLoader;
    std::vector<std::vector<std::string>> imuData;
    std::atomic<bool> running;
    std::thread sensorThread;
    std::function<void(double, double, double, double, double, double, double, double, double, double)> saveDataCallback;  // (time_stamp, x_acc, y_acc, z_acc, pitch, roll, yaw, x_gyro, y_gyro, z_gyro)
};

