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
    if (timeStamp < captureStartTime || captureStartTime < 0) return;

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
    dataBuffers[type].emplace_back(sensorData);
}

std::shared_ptr<SensorData> DataManager::getClosestData(SensorType type, double currentTime) {
    std::lock_guard<std::mutex> lock(sensorMutexes[type]);
    auto& buffer = dataBuffers[type];
    if (buffer.empty()) return nullptr;

    auto it = std::lower_bound(buffer.begin(), buffer.end(), currentTime,
        [this](const std::shared_ptr<SensorData>& data, double time) {
            return data->getTimestamp() < std::max(captureStartTime, time);
        });

    if (it == buffer.end()) return buffer.back();
    if (it == buffer.begin()) return buffer.front();

    auto prev = std::prev(it);
    return (currentTime - prev->getTimestamp() < it->getTimestamp() - currentTime) ? *prev : *it;
}

void DataManager::startCaptureFrom(double startTime) {
    captureStartTime = startTime;
}

void DataManager::setCaptureStartCallback(std::function<void(double)> callback) {
    captureStartCallback = callback;
}
