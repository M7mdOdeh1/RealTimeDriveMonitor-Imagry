#include "../include/RealTimeDriverMonitor.h"
#include <iostream>


RealTimeDriverMonitor::RealTimeDriverMonitor(const std::string& dataDir)
    : monitoringStarted(false),
      dataManager(new DataManager()),
      clockSystem(new ClockSystem()),
      cameraSystem(dataDir + "/3d_images", dataManager),
      brakeSensor(dataDir + "/brake.csv", [&](double time_stamp, double brake_pressure) {
          dataManager->addData(SensorType::BrakeSensor, time_stamp, { brake_pressure });
      }),
      gpsSensor(dataDir + "/gps.csv", [&](double time_stamp, double latitude, double longitude, double height) {
          dataManager->addData(SensorType::GPSSensor, time_stamp, std::vector<double>{latitude, longitude, height});
      }),
      speedSensor(dataDir + "/speed.csv", [&](double time_stamp, double speed) {
          dataManager->addData(SensorType::SpeedSensor, time_stamp, { speed });
      }),
      throttleSensor(dataDir + "/throttle.csv", [&](double time_stamp, double throttle_value) {
          dataManager->addData(SensorType::ThrottleSensor, time_stamp, { throttle_value });
      }),
      steeringSensor(dataDir + "/steering.csv", [&](double time_stamp, double steering_angle) {
          dataManager->addData(SensorType::SteeringSensor, time_stamp, { steering_angle });
      }),
      imuSensor(dataDir + "/imu.csv", [&](double time_stamp, double x_acc, double y_acc, double z_acc, double pitch, double roll, double yaw, double x_gyro, double y_gyro, double z_gyro) {
            dataManager->addData(SensorType::IMUSensor, time_stamp, std::vector<double>{x_acc, y_acc, z_acc, pitch, roll, yaw, x_gyro, y_gyro, z_gyro});
      }) 
{
    dataManager->setCaptureStartCallback([this](double startTime) {
        clockSystem->setCurrentTime(startTime);
    });
}


void RealTimeDriverMonitor::startMonitoring() {
    
    monitoringStarted = true;
    std::cout << "Monitoring Started" << std::endl;


    // Start all sensors and camera system
    brakeSensor.start();
    gpsSensor.start();
    speedSensor.start();
    throttleSensor.start();
    steeringSensor.start();
    imuSensor.start();
    
    cameraSystem.start(1000.0/30);  // 30 FPS 



    // Start the clock system
    clockSystem->start(1000.0/5, [this](double currentTime) { tick(currentTime); });  // 100ms tick interval
}

void RealTimeDriverMonitor::stopMonitoring() {
    monitoringStarted = false;

    // Stop all sensors and camera system
    brakeSensor.stop();
    gpsSensor.stop();
    speedSensor.stop();
    throttleSensor.stop();
    steeringSensor.stop();
    imuSensor.stop();
    cameraSystem.stop();

    // Stop the clock system
    clockSystem->stop();
}

void RealTimeDriverMonitor::tick(double currentTime) {
    if (!monitoringStarted) return;

    try {
        // Retrieve camera frames
        auto frontFrameOpt = getSensorDataOrNull<cv::Mat>(SensorType::FrontCamera, currentTime);
        auto leftFrameOpt = getSensorDataOrNull<cv::Mat>(SensorType::LeftCamera, currentTime);
        auto rightFrameOpt = getSensorDataOrNull<cv::Mat>(SensorType::RightCamera, currentTime);
        auto backFrameOpt = getSensorDataOrNull<cv::Mat>(SensorType::BackCamera, currentTime);

        // Only emit framesUpdated signal if all frames are available
        if (frontFrameOpt && leftFrameOpt && rightFrameOpt && backFrameOpt) {
            emit framesUpdated(frontFrameOpt.value(), leftFrameOpt.value(), rightFrameOpt.value(), backFrameOpt.value());
        }

        // Retrieve sensor data and handle null cases
        QString brakeText = "Brake Sensor: N/A";
        QString gpsText = "GPS Data: N/A";
        QString speedText = "Speed Sensor: N/A";
        QString throttleText = "Throttle: N/A";
        QString steeringText = "Steering: N/A";
        QString imuText = "IMU Sensor: N/A";

        auto brakeDataOpt = getSensorDataOrNull<double>(SensorType::BrakeSensor, currentTime);
        if (brakeDataOpt) {
            brakeText = QString("Brake Sensor: %1").arg(brakeDataOpt.value());
        }

        auto gpsDataOpt = getSensorDataOrNull<std::vector<double>>(SensorType::GPSSensor, currentTime);
        if (gpsDataOpt) {
            const auto& gpsData = gpsDataOpt.value();
            gpsText = QString("%1,%2,%3").arg(gpsData[0]).arg(gpsData[1]).arg(gpsData[2]);
        }

        auto speedDataOpt = getSensorDataOrNull<double>(SensorType::SpeedSensor, currentTime);
        if (speedDataOpt) {
            speedText = QString("Speed Sensor: %1").arg(speedDataOpt.value());
        }

        auto throttleDataOpt = getSensorDataOrNull<double>(SensorType::ThrottleSensor, currentTime);
        if (throttleDataOpt) {
            throttleText = QString("Throttle: %1").arg(throttleDataOpt.value());
        }

        auto steeringDataOpt = getSensorDataOrNull<double>(SensorType::SteeringSensor, currentTime);
        if (steeringDataOpt) {
            steeringText = QString("Steering: %1").arg(steeringDataOpt.value());
        }

        auto imuDataOpt = getSensorDataOrNull<std::vector<double>>(SensorType::IMUSensor, currentTime);
        if (imuDataOpt) {
            const auto& imuData = imuDataOpt.value();
            imuText = QString("Acc: (%1, %2, %3)\nGyro: (%4, %5, %6)\nMag: (%7, %8, %9)")
                            .arg(imuData[0]).arg(imuData[1]).arg(imuData[2])
                            .arg(imuData[6]).arg(imuData[7]).arg(imuData[8])
                            .arg(imuData[3]).arg(imuData[4]).arg(imuData[5]);
        }

        // Emit signals to update the UI for sensors
        emit sensorDataUpdated(brakeText, gpsText, speedText, throttleText, steeringText, imuText);

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

template<typename T>
std::optional<T> RealTimeDriverMonitor::getSensorDataOrNull(SensorType type, double currentTime) {
    auto closestData = dataManager->getClosestData(type, currentTime);
    if (!closestData) {
        std::cerr << "No closest data available for " << SensorTypeHelper::toString(type) << "." << std::endl;
        return std::nullopt;
    }
    auto sensorData = std::dynamic_pointer_cast<SensorData>(closestData);
    if (!sensorData) {
        std::cerr << "Failed to cast to SensorData." << std::endl;
        return std::nullopt;
    }
    try {
        return std::any_cast<T>(sensorData->getData());
    } catch (const std::bad_any_cast& e) {
        std::cerr << "Failed to cast sensor data: " << e.what() << std::endl;
        return std::nullopt;
    }
}
