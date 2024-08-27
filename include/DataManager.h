#pragma once

#include <opencv2/opencv.hpp>
#include <map>
#include <deque>
#include <mutex>
#include <string>
#include <vector>
#include <any>
#include <memory>
#include <functional>
#include <array>
#include "include/Sensors/SensorType.h"
#include "include/SensorData/SensorData.h"

class DataManager {
public:
    DataManager();

    // Unified method for adding data (sensor data and camera frames)
    void addData(SensorType sensorType, double timeStamp, const std::any& data);
    
    // Unified method for getting the closest data (sensor data and camera frames)
    std::shared_ptr<SensorData> getClosestData(SensorType sensorType, double currentTime);

    void startCaptureFrom(double startTime);  // Set the starting point for data capture
    void setCaptureStartCallback(std::function<void(double)> callback);

private:
    const size_t MAX_BUFFER_SIZE = 100;  // Maximum size for each sensor's buffer

    std::map<SensorType, std::deque<std::shared_ptr<SensorData>>> dataBuffers;

    std::map<SensorType, std::mutex> sensorMutexes;

    std::function<void(double)> captureStartCallback;  
    
    double captureStartTime;
};
