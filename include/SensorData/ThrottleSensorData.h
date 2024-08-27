#pragma once

#include "SensorData.h"
#include <iostream>

class ThrottleSensorData : public SensorData {
public:
    ThrottleSensorData(double ts, double throttleValue) : SensorData(ts), throttleValue(throttleValue) {}

    void setData(const std::any& data) override {
        try {
            this->throttleValue = std::any_cast<double>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to double): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return throttleValue;
    }

private:
    double throttleValue;
};
