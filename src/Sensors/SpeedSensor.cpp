#include "Sensors/SpeedSensor.h"
#include <chrono>

SpeedSensor::SpeedSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback)
    : csvLoader(csvFilePath), running(false), saveDataCallback(saveDataCallback) 
{
    speedData = csvLoader.loadCSV();
}

SpeedSensor::~SpeedSensor() {
    stop();
}

void SpeedSensor::start() {
    running = true;
    sensorThread = std::thread(&SpeedSensor::readDataLoop, this);
}

void SpeedSensor::stop() {
    running = false;
    if (sensorThread.joinable()) {
        sensorThread.join();
    }
}

void SpeedSensor::readDataLoop() {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= speedData.size()) {
            currentIndex = 0;  // if we've reached the end
        }

        // time_stamp is in the first column and speed in the second
        double time_stamp = std::stod(speedData[currentIndex][0]);
        double speed = std::stod(speedData[currentIndex][1]);
        saveDataCallback(time_stamp, speed);

        currentIndex++;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
