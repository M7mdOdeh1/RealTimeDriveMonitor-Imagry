#include "Sensors/BrakeSensor.h"
#include <chrono>
#include <iostream>

BrakeSensor::BrakeSensor(const std::string& csvFilePath, std::function<void(double, double)> saveDataCallback)
    : csvLoader(csvFilePath), running(false), saveDataCallback(saveDataCallback) 
{
    brakeData = csvLoader.loadCSV();
}

BrakeSensor::~BrakeSensor() {
    stop();
}

void BrakeSensor::start() {
    running = true;
    sensorThread = std::thread(&BrakeSensor::readDataLoop, this);
}

void BrakeSensor::stop() {
    running = false;
    if (sensorThread.joinable()) {
        sensorThread.join();
    }
}

void BrakeSensor::readDataLoop() {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= brakeData.size()) {
            currentIndex = 0;  // if we've reached the end
        }

        // time_stamp is in the first column and brake_pressure in the second
        double time_stamp = std::stod(brakeData[currentIndex][0]);
        double brakePressure = std::stod(brakeData[currentIndex][1]);
        saveDataCallback(time_stamp, brakePressure);


        currentIndex++;

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}
