#include "Sensors/ThrottleSensor.h"
#include <chrono>

ThrottleSensor::ThrottleSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback)
    : csvLoader(csvFilePath), running(false), saveDataCallback(saveDataCallback) 
{
    throttleData = csvLoader.loadCSV();
}

ThrottleSensor::~ThrottleSensor() {
    stop();
}

void ThrottleSensor::start() {
    running = true;
    sensorThread = std::thread(&ThrottleSensor::readDataLoop, this);
}

void ThrottleSensor::stop() {
    running = false;
    if (sensorThread.joinable()) {
        sensorThread.join();
    }
}

void ThrottleSensor::readDataLoop() {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= throttleData.size()) {
            currentIndex = 0;  //if we've reached the end
        }

        // time_stamp is in the first column and throttle_value in the second
        double time_stamp = std::stod(throttleData[currentIndex][0]);
        double throttleValue = std::stod(throttleData[currentIndex][1]);
        saveDataCallback(time_stamp, throttleValue);

        currentIndex++;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
