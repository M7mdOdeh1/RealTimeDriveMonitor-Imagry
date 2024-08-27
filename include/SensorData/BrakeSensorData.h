#pragma once

#include "SensorData.h"
#include <iostream>

class BrakeSensorData : public SensorData {
public:
    BrakeSensorData(double ts, double brakeValue) : SensorData(ts), brakeValue(brakeValue) {}

    void setData(const std::any& data) override {
        try {
            this->brakeValue = std::any_cast<double>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to double): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return brakeValue;
    }

private:
    double brakeValue;
};
