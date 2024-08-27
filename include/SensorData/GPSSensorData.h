#pragma once

#include "SensorData.h"
#include <iostream>
#include <vector>


#include <vector>

class GPSSensorData : public SensorData {
public:
    GPSSensorData(double ts, const std::vector<double>& gpsValues) : SensorData(ts) {
        if (gpsValues.size() != 3) {
            throw std::invalid_argument("GPS data must contain 2 values (latitude, longitude, height)");
        }
    }

    void setData(const std::any& data) override {
        try {
            this->gpsValues = std::any_cast<std::vector<double>>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to std::vector<double>): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return gpsValues;
    }

private:
    std::vector<double> gpsValues;
};
