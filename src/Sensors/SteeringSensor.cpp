#include "Sensors/SteeringSensor.h"
#include <chrono>

SteeringSensor::SteeringSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback)
    : csvLoader(csvFilePath), running(false), saveDataCallback(saveDataCallback) 
{
    steeringData = csvLoader.loadCSV();
}

SteeringSensor::~SteeringSensor() {
    stop();
}

void SteeringSensor::start() {
    running = true;
    sensorThread = std::thread(&SteeringSensor::readDataLoop, this);
}

void SteeringSensor::stop() {
    running = false;
    if (sensorThread.joinable()) {
        sensorThread.join();
    }
}

void SteeringSensor::readDataLoop() {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= steeringData.size()) {
            currentIndex = 0;  // if we've reached the end
        }

        // time_stamp is in the first column and steering_angle in the second
        double time_stamp = std::stod(steeringData[currentIndex][0]);
        double steeringAngle = std::stod(steeringData[currentIndex][1]);
        saveDataCallback(time_stamp, steeringAngle);

        currentIndex++;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
