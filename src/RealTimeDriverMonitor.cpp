#include "../include/RealTimeDriverMonitor.h"

RealTimeDriverMonitor::RealTimeDriverMonitor(const std::string& dataDir)
    : monitoringStarted(false),
      dataManager(new DataManager()),
      clockSystem(new ClockSystem()),
      cameraSystem(dataDir + "/3d_images", dataManager),
      brakeSensor(dataDir + "/brake.csv", [&](double time_stamp, double brake_pressure) {
          dataManager->addSensorData(SensorType::BrakeSensor, time_stamp, { brake_pressure });
      }),
      gpsSensor(dataDir + "/gps.csv", [&](double time_stamp, double latitude, double longitude, double height) {
          dataManager->addSensorData(SensorType::GPSSensor, time_stamp, { latitude, longitude, height });
      }),
      speedSensor(dataDir + "/speed.csv", [&](double time_stamp, double speed) {
          dataManager->addSensorData(SensorType::SpeedSensor, time_stamp, { speed });
      }),
      throttleSensor(dataDir + "/throttle.csv", [&](double time_stamp, double throttle_value) {
          dataManager->addSensorData(SensorType::ThrottleSensor, time_stamp, { throttle_value });
      }),
      steeringSensor(dataDir + "/steering.csv", [&](double time_stamp, double steering_angle) {
          dataManager->addSensorData(SensorType::SteeringSensor, time_stamp, { steering_angle });
      }),
      imuSensor(dataDir + "/imu.csv", [&](double time_stamp, double x_acc, double y_acc, double z_acc, double pitch, double roll, double yaw, double x_gyro, double y_gyro, double z_gyro) {
          dataManager->addSensorData(SensorType::IMUSensor, time_stamp, { x_acc, y_acc, z_acc, pitch, roll, yaw, x_gyro, y_gyro, z_gyro });
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
    clockSystem->start(30, [this](double currentTime) { tick(currentTime); });  // 100ms tick interval
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



    auto frontFrame = dataManager->getClosestCameraFrame(SensorType::FrontCamera, currentTime).second;
    auto leftFrame = dataManager->getClosestCameraFrame(SensorType::LeftCamera, currentTime).second;
    auto rightFrame = dataManager->getClosestCameraFrame(SensorType::RightCamera, currentTime).second;
    auto backFrame = dataManager->getClosestCameraFrame(SensorType::BackCamera, currentTime).second;

    auto brakeData = dataManager->getClosestSensorData(SensorType::BrakeSensor, currentTime).second;
    auto gpsData = dataManager->getClosestSensorData(SensorType::GPSSensor, currentTime).second;
    auto speedData = dataManager->getClosestSensorData(SensorType::SpeedSensor, currentTime).second;
    auto throttleData = dataManager->getClosestSensorData(SensorType::ThrottleSensor, currentTime).second;
    auto steeringData = dataManager->getClosestSensorData(SensorType::SteeringSensor, currentTime).second;
    auto imuData = dataManager->getClosestSensorData(SensorType::IMUSensor, currentTime).second;

    if (frontFrame.empty() || leftFrame.empty() || rightFrame.empty() || backFrame.empty()) {
        //std::cout << "No frames available" << std::endl;
        return;
    }

    if (brakeData.empty() || gpsData.empty() || speedData.empty() || throttleData.empty() || steeringData.empty() || imuData.empty()) {
        //std::cout << "No sensor data available" << std::endl;
        return;
    }

    // Emit signals to update the UI 
    emit framesUpdated(frontFrame, leftFrame, rightFrame, backFrame);

    QString brakeText = QString("Brake Sensor: %1").arg(brakeData[0]);
    QString gpsText = QString("%1,%2,%3").arg(gpsData[0]).arg(gpsData[1]).arg(gpsData[2]);
    QString speedText = QString("Speed Sensor: %1").arg(speedData[0]);
    QString throttleText = QString("Throttle: %1").arg(throttleData[0]);
    QString steeringText = QString("Steering: %1").arg(steeringData[0]);
    QString imuText = QString("Acc: (%1, %2, %3), Gyro: (%4, %5, %6), Mag: (%7, %8, %9)")
                        .arg(imuData[0]).arg(imuData[1]).arg(imuData[2])
                        .arg(imuData[6]).arg(imuData[7]).arg(imuData[8])
                        .arg(imuData[3]).arg(imuData[4]).arg(imuData[5]);

    emit sensorDataUpdated(brakeText, gpsText, speedText, throttleText, steeringText, imuText);
    
    
}
