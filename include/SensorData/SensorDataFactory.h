// SensorDataFactory.h
#pragma once
#include <any>
#include <memory>
#include "SensorData.h"
#include "SpeedSensorData.h"
#include "SteeringSensorData.h"
#include "ThrottleSensorData.h"
#include "BrakeSensorData.h"
#include "GPSSensorData.h"
#include "IMUSensorData.h"
#include "CameraSensorData.h"
#include "Sensors/SensorType.h"

class SensorDataFactory {
public:
    static std::shared_ptr<SensorData> createSensorData(SensorType type, double timeStamp, const std::any& data) {
        try {
            switch (type) {
                case SensorType::SpeedSensor:
                    return std::make_shared<SpeedSensorData>(timeStamp, std::any_cast<double>(data));
                case SensorType::SteeringSensor:
                    return std::make_shared<SteeringSensorData>(timeStamp, std::any_cast<double>(data));
                case SensorType::ThrottleSensor:
                    return std::make_shared<ThrottleSensorData>(timeStamp, std::any_cast<double>(data));
                case SensorType::BrakeSensor:
                    return std::make_shared<BrakeSensorData>(timeStamp, std::any_cast<double>(data));
                case SensorType::GPSSensor:
                    return std::make_shared<GPSSensorData>(timeStamp, std::any_cast<std::vector<double>>(data));
                case SensorType::IMUSensor:
                    return std::make_shared<IMUSensorData>(timeStamp, std::any_cast<std::vector<double>>(data));
                case SensorType::FrontCamera:
                case SensorType::BackCamera:
                case SensorType::LeftCamera:
                case SensorType::RightCamera:
                    return std::make_shared<CameraSensorData>(timeStamp, std::any_cast<cv::Mat>(data));
                default:
                    throw std::runtime_error("Unknown sensor type");
            }
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Failed to cast std::any: " + std::string(e.what()));
        }
    }
};
