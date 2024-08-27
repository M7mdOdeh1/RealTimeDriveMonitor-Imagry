#pragma once

#include "SensorData.h"
#include <iostream>

class SteeringSensorData : public SensorData {
public:
    SteeringSensorData(double ts, double steeringAngle) : SensorData(ts), steeringAngle(steeringAngle) {}

    void setData(const std::any& data) override {
        try {
            this->steeringAngle = std::any_cast<double>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to double): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return steeringAngle;
    }

private:
    double steeringAngle;
};
