#pragma once

#include <any>
#include <iostream>
#include <stdexcept>

class SensorData {
public:
    SensorData(double ts) : timestamp(ts) {}

    virtual void setData(const std::any& data) {
        this->data = data;
    }

    virtual std::any getData() const {
        return data;
    }

    double getTimestamp() const {
        return timestamp;
    }

    virtual void printData() const {
        std::cout << "Timestamp: " << timestamp << ", Data: " << data.type().name() << std::endl;
    }

protected:
    double timestamp;
    std::any data;
};
