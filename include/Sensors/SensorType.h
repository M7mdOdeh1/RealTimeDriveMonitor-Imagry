#pragma once

#include <string>

enum class SensorType {
    FrontCamera,
    LeftCamera,
    RightCamera,
    BackCamera,
    BrakeSensor,
    GPSSensor,
    SpeedSensor,
    ThrottleSensor,
    SteeringSensor,
    IMUSensor
};

class SensorTypeHelper {
public:
    static std::string toString(SensorType type) {
        switch (type) {
            case SensorType::FrontCamera: return "FrontCamera";
            case SensorType::LeftCamera: return "LeftCamera";
            case SensorType::RightCamera: return "RightCamera";
            case SensorType::BackCamera: return "BackCamera";
            case SensorType::BrakeSensor: return "BrakeSensor";
            case SensorType::GPSSensor: return "GPSSensor";
            case SensorType::SpeedSensor: return "SpeedSensor";
            case SensorType::ThrottleSensor: return "ThrottleSensor";
            case SensorType::SteeringSensor: return "SteeringSensor";
            case SensorType::IMUSensor: return "IMUSensor";
            default: return "UnknownSensor";
        }
    }
};
