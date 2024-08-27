cmake_minimum_required(VERSION 3.5)

project(RealTimeDriveMonitor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages, including WebEngineWidgets
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets WebEngineWidgets REQUIRED)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add your source files here
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    src/Sensors/BrakeSensor.cpp
    src/Camera.cpp
    src/CameraSystem.cpp
    src/ClockSystem.cpp
    src/Loaders/CSVLoader.cpp
    src/DataManager.cpp
    src/Sensors/GPSSensor.cpp
    src/Sensors/IMUSensor.cpp
    src/RealTimeDriverMonitor.cpp
    src/Sensors/SpeedSensor.cpp
    src/Sensors/SteeringSensor.cpp
    src/Sensors/ThrottleSensor.cpp
    src/Loaders/ImageLoader.cpp
)

set(PROJECT_HEADERS
    include/Sensors/BrakeSensor.h
    include/Camera.h
    include/CameraSystem.h
    include/ClockSystem.h
    include/Loaders/CSVLoader.h
    include/DataManager.h
    include/Sensors/GPSSensor.h
    include/Sensors/IMUSensor.h
    include/RealTimeDriverMonitor.h
    include/Sensors/SensorType.h
    include/Sensors/SpeedSensor.h
    include/Sensors/SteeringSensor.h
    include/Sensors/ThrottleSensor.h
    include/Loaders/ImageLoader.h

    include/SensorData/BrakeSensorData.h
    include/SensorData/GPSSensorData.h
    include/SensorData/IMUSensorData.h
    include/SensorData/SensorData.h
    include/SensorData/SpeedSensorData.h
    include/SensorData/SteeringSensorData.h
    include/SensorData/ThrottleSensorData.h
    include/SensorData/CameraSensorData.h
    
)

set(PROJECT_UI
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RealTimeDriveMonitor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UI}
    )
else()
    if(ANDROID)
        add_library(RealTimeDriveMonitor SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${PROJECT_UI}
        )
    else()
        add_executable(RealTimeDriveMonitor
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${PROJECT_UI}
        )
    endif()
endif()

# Link OpenCV and Qt libraries, including WebEngineWidgets
target_link_libraries(RealTimeDriveMonitor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets ${OpenCV_LIBS})

set_target_properties(RealTimeDriveMonitor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RealTimeDriveMonitor)
endif()
