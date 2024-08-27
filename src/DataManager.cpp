#include "../include/DataManager.h"
#include "../include/SensorData/SpeedSensorData.h"
#include "../include/SensorData/SteeringSensorData.h"
#include "../include/SensorData/ThrottleSensorData.h"
#include "../include/SensorData/BrakeSensorData.h"
#include "../include/SensorData/GPSSensorData.h"
#include "../include/SensorData/IMUSensorData.h"
#include "../include/SensorData/CameraSensorData.h"
#include "../include/SensorData/SensorData.h"
#include <algorithm>
#include <stdexcept>

DataManager::DataManager() : captureStartTime(-1.0), captureStartCallback(nullptr) {}

void DataManager::addData(SensorType type, double timeStamp, const std::any& data) {
    //std::cout << "Adding data of type " << static_cast<int>(type) << " at time " << timeStamp << std::endl;

    if (captureStartTime < 0 && SensorType::FrontCamera == type) {
        startCaptureFrom(timeStamp);
    }

    //if (timeStamp < captureStartTime || captureStartTime < 0) return;

    std::shared_ptr<SensorData> sensorData;

    try {
        switch (type) {
            case SensorType::SpeedSensor:
                sensorData = std::make_shared<SpeedSensorData>(timeStamp, std::any_cast<double>(data));
                break;
            case SensorType::SteeringSensor:
                sensorData = std::make_shared<SteeringSensorData>(timeStamp, std::any_cast<double>(data));
                break;
            case SensorType::ThrottleSensor:
                sensorData = std::make_shared<ThrottleSensorData>(timeStamp, std::any_cast<double>(data));
                break;
            case SensorType::BrakeSensor:
                //std::cout << "Brake sensor data: " << std::any_cast<double>(data) << std::endl;
                sensorData = std::make_shared<BrakeSensorData>(timeStamp, std::any_cast<double>(data));
                break;
            case SensorType::GPSSensor:
                sensorData = std::make_shared<GPSSensorData>(timeStamp, std::any_cast<std::vector<double>>(data));
                break;
            case SensorType::IMUSensor:
                sensorData = std::make_shared<IMUSensorData>(timeStamp, std::any_cast<std::vector<double>>(data));
                break;
            case SensorType::FrontCamera:
            case SensorType::BackCamera:
            case SensorType::LeftCamera:
            case SensorType::RightCamera:
                sensorData = std::make_shared<CameraSensorData>(timeStamp, std::any_cast<cv::Mat>(data));
                break;
            default:
                throw std::runtime_error("Unknown sensor type");
        }
    } catch (const std::bad_any_cast& e) {
        throw std::runtime_error("Failed to cast std::any: " + std::string(e.what()));
    }

    std::lock_guard<std::mutex> lock(sensorMutexes[type]);
    if (dataBuffers[type].size() >= MAX_BUFFER_SIZE) {
        std::cout << "Buffer for sensor type " << static_cast<int>(type) << " is full. Removing oldest data." << std::endl;
        dataBuffers[type].pop_front();  // Remove the oldest data
    }
    dataBuffers[type].emplace_back(sensorData);
}

std::shared_ptr<SensorData> DataManager::getClosestData(SensorType type, double currentTime) {
    std::lock_guard<std::mutex> lock(sensorMutexes[type]);
    auto& buffer = dataBuffers[type];
    if (buffer.empty()) return nullptr;

    // if (captureStartTime < 0) {
    //     return nullptr;
    // }

    // // Remove all data before captureStartTime
    // auto removeIt = std::remove_if(buffer.begin(), buffer.end(), 
    //     [this](const std::shared_ptr<SensorData>& data) {
    //         return data->getTimestamp() < captureStartTime;
    //     });

    // // Erase the unnecessary data
    // buffer.erase(buffer.begin(), removeIt);

    // // After removal, if the buffer is empty, return nullptr
    // if (buffer.empty()) return nullptr;

    // Now find the closest data point to currentTime
    auto it = std::lower_bound(buffer.begin(), buffer.end(), currentTime,
        [](const std::shared_ptr<SensorData>& data, double time) {
            return data->getTimestamp() < time;
        });

    if (it == buffer.end()){
        // All timestamps are less than currentTime, return the last element
        auto closest = buffer.back();

        // Clear all elements before the last one, since they are no longer needed
        buffer.clear();
        buffer.push_back(closest);

        return closest;
    } 

    if (it == buffer.begin()) {
        return *it;         // All timestamps are greater than currentTime, return the first element

    }

    auto prev = std::prev(it);
    
    if (std::abs((*it)->getTimestamp() - currentTime) < std::abs((*prev)->getTimestamp() - currentTime)) {
        return *it;
    } else {
        std::cout << "prev" << std::endl;
        return *prev;
    }
}

void DataManager::startCaptureFrom(double startTime) {
    captureStartTime = startTime;
    if (captureStartCallback) {
        captureStartCallback(startTime);
    }
    std::cout << "Capture start time set to " << startTime << std::endl;
}

void DataManager::setCaptureStartCallback(std::function<void(double)> callback) {
    captureStartCallback = callback;
}
