#include "../include/RealTimeDriverMonitor.h"

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
    
    cameraSystem.start(1000.0 / 30.0);  // 30 FPS 



    // Start the clock system
    clockSystem->start(1000.0/30, [this](double currentTime) { tick(currentTime); });  // 100ms tick interval
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
    //std::cout << "Current Time: " << currentTime << std::endl;

    if (!monitoringStarted) return;


    // auto frontFrame = dataManager->getClosestData(SensorType::FrontCamera, currentTime).second;
    // auto leftFrame = dataManager->getClosestData(SensorType::LeftCamera, currentTime).second;
    // auto rightFrame = dataManager->getClosestData(SensorType::RightCamera, currentTime).second;
    // auto backFrame = dataManager->getClosestData(SensorType::BackCamera, currentTime).second;

    // auto brakeData = dataManager->getClosestData(SensorType::BrakeSensor, currentTime).second;
    // auto gpsData = dataManager->getClosestData(SensorType::GPSSensor, currentTime).second;
    // auto speedData = dataManager->getClosestData(SensorType::SpeedSensor, currentTime).second;
    // auto throttleData = dataManager->getClosestData(SensorType::ThrottleSensor, currentTime).second;
    // auto steeringData = dataManager->getClosestData(SensorType::SteeringSensor, currentTime).second;
    // auto imuData = dataManager->getClosestData(SensorType::IMUSensor, currentTime).second;

    try {
        // Retrieve and cast data from CameraSensorData
        auto closestData = dataManager->getClosestData(SensorType::FrontCamera, currentTime);
        if (!closestData) {
            std::cerr << "No closest data available for FrontCamera." << std::endl;
            return;
        }
        auto cameraData = std::dynamic_pointer_cast<CameraSensorData>(closestData);
        if (!cameraData) {
            std::cerr << "Failed to cast to CameraSensorData." << std::endl;
            return;
        }
        auto frontFrame = std::any_cast<cv::Mat>(cameraData->getData());

        closestData = dataManager->getClosestData(SensorType::LeftCamera, currentTime);
        if (!closestData) {
            std::cerr << "No closest data available for LeftCamera." << std::endl;
            return;
        }
        cameraData = std::dynamic_pointer_cast<CameraSensorData>(closestData);
        if (!cameraData) {
            std::cerr << "Failed to cast to CameraSensorData." << std::endl;
            return;
        }
        auto leftFrame = std::any_cast<cv::Mat>(cameraData->getData());

        closestData = dataManager->getClosestData(SensorType::RightCamera, currentTime);
        if (!closestData) {
            std::cerr << "No closest data available for RightCamera." << std::endl;
            return;
        }
        cameraData = std::dynamic_pointer_cast<CameraSensorData>(closestData);
        if (!cameraData) {
            std::cerr << "Failed to cast to CameraSensorData." << std::endl;
            return;
        }
        auto rightFrame = std::any_cast<cv::Mat>(cameraData->getData());

        closestData = dataManager->getClosestData(SensorType::BackCamera, currentTime);
        if (!closestData) {
            std::cerr << "No closest data available for BackCamera." << std::endl;
            return;
        }
        cameraData = std::dynamic_pointer_cast<CameraSensorData>(closestData);
        if (!cameraData) {
            std::cerr << "Failed to cast to CameraSensorData." << std::endl;
            return;
        }
        auto backFrame = std::any_cast<cv::Mat>(cameraData->getData());


        

        // Retrieve and cast sensor data
        auto brakeSensorData = std::dynamic_pointer_cast<BrakeSensorData>(dataManager->getClosestData(SensorType::BrakeSensor, currentTime));
        if (!brakeSensorData) {
            std::cerr << "No closest data available for BrakeSensor." << std::endl;
            return;
        }
        auto brakeData = std::any_cast<double>(brakeSensorData->getData());

        auto gpsSensorData = std::dynamic_pointer_cast<GPSSensorData>(dataManager->getClosestData(SensorType::GPSSensor, currentTime));
        if (!gpsSensorData) {
            std::cerr << "No closest data available for GPSSensor." << std::endl;
            return;
        }
        auto gpsData = std::any_cast<std::vector<double>>(gpsSensorData->getData());

        auto speedSensorData = std::dynamic_pointer_cast<SpeedSensorData>(dataManager->getClosestData(SensorType::SpeedSensor, currentTime));
        if (!speedSensorData) {
            std::cerr << "No closest data available for SpeedSensor." << std::endl;
            return;
        }
        auto speedData = std::any_cast<double>(speedSensorData->getData());

        auto throttleSensorData = std::dynamic_pointer_cast<ThrottleSensorData>(dataManager->getClosestData(SensorType::ThrottleSensor, currentTime));
        if (!throttleSensorData) {
            std::cerr << "No closest data available for ThrottleSensor." << std::endl;
            return;
        }
        auto throttleData = std::any_cast<double>(throttleSensorData->getData());

        auto steeringSensorData = std::dynamic_pointer_cast<SteeringSensorData>(dataManager->getClosestData(SensorType::SteeringSensor, currentTime));
        if (!steeringSensorData) {
            std::cerr << "No closest data available for SteeringSensor." << std::endl;
            return;
        }
        auto steeringData = std::any_cast<double>(steeringSensorData->getData());

        auto imuSensorData = std::dynamic_pointer_cast<IMUSensorData>(dataManager->getClosestData(SensorType::IMUSensor, currentTime));
        if (!imuSensorData) {
            std::cerr << "No closest data available for IMUSensor." << std::endl;
            return;
        }
        auto imuData = std::any_cast<std::vector<double>>(imuSensorData->getData());

        // Emit signals to update the UI

        emit framesUpdated(frontFrame, leftFrame, rightFrame, backFrame);

        QString brakeText = QString("Brake Sensor: %1").arg(brakeData);
        QString gpsText = QString("%1,%2,%3").arg(gpsData[0]).arg(gpsData[1]).arg(gpsData[2]);
        QString speedText = QString("Speed Sensor: %1").arg(speedData);
        QString throttleText = QString("Throttle: %1").arg(throttleData);
        QString steeringText = QString("Steering: %1").arg(steeringData);
        QString imuText = QString("Acc: (%1, %2, %3)\nGyro: (%4, %5, %6)\nMag: (%7, %8, %9)")
                            .arg(imuData[0]).arg(imuData[1]).arg(imuData[2])
                            .arg(imuData[6]).arg(imuData[7]).arg(imuData[8])
                            .arg(imuData[3]).arg(imuData[4]).arg(imuData[5]);

        emit sensorDataUpdated(brakeText, gpsText, speedText, throttleText, steeringText, imuText);

    } catch (const std::bad_any_cast& e) {
        std::cerr << "Failed to cast sensor data: " << e.what() << std::endl;
        return;
    }

}
