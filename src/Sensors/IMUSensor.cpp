#include "Sensors/IMUSensor.h"
#include <chrono>

IMUSensor::IMUSensor(const std::string& csvFilePath, std::function<void(double, double, double, double, double, double, double, double, double, double)> saveDataCallback)
    : csvLoader(csvFilePath), running(false), saveDataCallback(saveDataCallback) 
{
    imuData = csvLoader.loadCSV();
}

IMUSensor::~IMUSensor() {
    stop();
}

void IMUSensor::start() {
    running = true;
    sensorThread = std::thread(&IMUSensor::readDataLoop, this);
}

void IMUSensor::stop() {
    running = false;
    if (sensorThread.joinable()) {
        sensorThread.join();
    }
}

void IMUSensor::readDataLoop() {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= imuData.size()) {
            currentIndex = 0;  // if we've reached the end
        }

        // time_stamp, x_acc, y_acc, z_acc, pitch, roll, yaw, x_gyro, y_gyro, z_gyro are in the appropriate columns
        double time_stamp = std::stod(imuData[currentIndex][0]);
        double x_acc = std::stod(imuData[currentIndex][1]);
        double y_acc = std::stod(imuData[currentIndex][2]);
        double z_acc = std::stod(imuData[currentIndex][3]);
        double pitch = std::stod(imuData[currentIndex][4]);
        double roll = std::stod(imuData[currentIndex][5]);
        double yaw = std::stod(imuData[currentIndex][6]);
        double x_gyro = std::stod(imuData[currentIndex][7]);
        double y_gyro = std::stod(imuData[currentIndex][8]);
        double z_gyro = std::stod(imuData[currentIndex][9]);

        saveDataCallback(time_stamp, x_acc, y_acc, z_acc, pitch, roll, yaw, x_gyro, y_gyro, z_gyro);

        currentIndex++;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
