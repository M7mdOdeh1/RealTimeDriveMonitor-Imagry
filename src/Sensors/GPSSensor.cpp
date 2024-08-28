#include "Sensors/GPSSensor.h"
#include <chrono>

GPSSensor::GPSSensor(const std::string& csvFilePath, std::function<void(double, double, double, double)> saveDataCallback)
    : csvLoader(csvFilePath), running(false), saveDataCallback(saveDataCallback) 
{
    gpsData = csvLoader.loadCSV();
}

GPSSensor::~GPSSensor() {
    stop();
}

void GPSSensor::start() {
    running = true;
    sensorThread = std::thread(&GPSSensor::readDataLoop, this);
}

void GPSSensor::stop() {
    running = false;
    if (sensorThread.joinable()) {
        sensorThread.join();
    }
}

void GPSSensor::readDataLoop() {
    int currentIndex = 0;
    while (running) {
        if (currentIndex >= gpsData.size()) {
            currentIndex = 0;  //if we've reached the end
        }

        // time_stamp, latitude, longitude, and height are in the first four columns
        double time_stamp = std::stod(gpsData[currentIndex][0]);
        double latitude = std::stod(gpsData[currentIndex][1]);
        double longitude = std::stod(gpsData[currentIndex][2]);
        double height = std::stod(gpsData[currentIndex][3]);
        saveDataCallback(time_stamp, latitude, longitude, height);

        currentIndex++;

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}
