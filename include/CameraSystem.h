#pragma once

#include "Camera.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <functional>
#include "DataManager.h"
#include "include/Sensors/SensorType.h"

class CameraSystem {
public:
    CameraSystem(const std::string& baseDir, DataManager* dataManager);
    
    void start(double frameTime);
    void stop();

private:
    Camera frontCamera;
    Camera leftCamera;
    Camera rightCamera;
    Camera backCamera;
};

