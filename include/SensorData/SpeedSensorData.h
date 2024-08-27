#pragma once

#include "SensorData.h"
#include <iostream>


class SpeedSensorData : public SensorData {
public:
    SpeedSensorData(double ts, double speedValue) : SensorData(ts), speedValue(speedValue) {}

    void setData(const std::any& data) override {
        try {
            this->speedValue = std::any_cast<double>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to double): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return speedValue;
    }

private:
    double speedValue;
};
