#pragma once

#include "SensorData.h"
#include <iostream>
#include <array>

#include <vector>

class IMUSensorData : public SensorData {
public:
    IMUSensorData(double ts, const std::vector<double>& imuValues) : SensorData(ts) {
        if (imuValues.size() != 9) {
            throw std::invalid_argument("IMU data must contain 9 values (acceleration, angular velocity, orientation)");
        }

    }

    void setData(const std::any& data) override {
        try {
            this->imuValues = std::any_cast<std::vector<double>>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to std::vector<double>): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return imuValues;
    }

private:
    std::vector<double> imuValues;
};
