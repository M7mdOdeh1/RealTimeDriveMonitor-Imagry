#pragma once

#include "SensorData.h"
#include <opencv2/opencv.hpp>


class CameraSensorData : public SensorData {
public:
    CameraSensorData(double ts, const cv::Mat& frame) : SensorData(ts), frame(frame) {}

    void setData(const std::any& data) override {
        try {
            this->frame = std::any_cast<cv::Mat>(data);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Casting failed (std::any to cv::Mat): " + std::string(e.what()));
        }
    }

    std::any getData() const override {
        return frame;
    }

private:
    cv::Mat frame;
};
