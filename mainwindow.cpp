#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Create the main layout
    QWidget *centralWidget = ui->centralwidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Set the window size
    this->setFixedSize(1920, 1080);

    // Create the sensor data layout (left side)
    QWidget *sensorWidget = new QWidget(this);
    QVBoxLayout *sensorLayout = new QVBoxLayout(sensorWidget);
    brakeLabel = new QLabel("Brake Data", this);
    gpsLabel = new QLabel("GPS Data", this);
    speedLabel = new QLabel("Speed Data", this);
    throttleLabel = new QLabel("Throttle Data", this);
    steeringLabel = new QLabel("Steering Data", this);
    imuLabel = new QLabel("IMU Data", this);

    sensorLayout->addWidget(brakeLabel);
    sensorLayout->addWidget(gpsLabel);
    sensorLayout->addWidget(speedLabel);
    sensorLayout->addWidget(throttleLabel);
    sensorLayout->addWidget(steeringLabel);
    sensorLayout->addWidget(imuLabel);
    


    // Set a fixed width for the sensorWidget
    sensorWidget->setFixedWidth(300);  // Adjust this value as needed
    sensorWidget->setFixedHeight(540);  // Adjust this value as needed
    sensorWidget->setStyleSheet("border: 1px solid red");


    // Create the camera layout (right side)
    QGridLayout *cameraLayout = new QGridLayout();
    frontCameraLabel = new QLabel(this);
    leftCameraLabel = new QLabel(this);
    rightCameraLabel = new QLabel(this);
    backCameraLabel = new QLabel(this);

    frontCameraLabel->setStyleSheet("border: 1px solid lime");
    leftCameraLabel->setStyleSheet("border: 1px solid lime");
    rightCameraLabel->setStyleSheet("border: 1px solid lime");
    backCameraLabel->setStyleSheet("border: 1px solid lime");

    // Set fixed size for each frame label
    frontCameraLabel->setFixedSize(695, 397);
    leftCameraLabel->setFixedSize(695, 397);
    rightCameraLabel->setFixedSize(695, 397);
    backCameraLabel->setFixedSize(695, 397);

    // Add widgets to the camera layout
    cameraLayout->addWidget(frontCameraLabel, 0, 1);
    cameraLayout->addWidget(leftCameraLabel, 0, 0);
    cameraLayout->addWidget(rightCameraLabel, 1, 0);
    cameraLayout->addWidget(backCameraLabel, 1, 1);

    // Add both layouts to the main layout
    mainLayout->addWidget(sensorWidget);  // Add the sensorWidget (containing sensorLayout)
    mainLayout->addLayout(cameraLayout);

    // Create the map view to display Google Maps
    // mapView = new QWebEngineView(this);
    // mapView->setFixedSize(900, 600);  // Adjust the size of the map view
    // mapView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    // mapView->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    // mapView->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);





    // mapView->setUrl(QUrl::fromLocalFile("/home/m7mdodeh/Desktop/imagry_training/qt_projects/RealTimeDriveMonitor/RealTimeDriveMonitor/map.html"));
    // //mainLayout->addWidget(mapView);  
    
    


    // Set the central widget
    this->setCentralWidget(centralWidget);

    driverMonitor = new RealTimeDriverMonitor("../RealTimeDriveMonitor/data");
    driverMonitor->startMonitoring();

    std::cout << "Initializing RealTimeDriverMonitor" << std::endl;

    connect(driverMonitor, &RealTimeDriverMonitor::framesUpdated, this, &MainWindow::updateCameraFrames);
    connect(driverMonitor, &RealTimeDriverMonitor::sensorDataUpdated, this, &MainWindow::updateSensorData);
}

MainWindow::~MainWindow() {
    delete driverMonitor;
    delete mapView;

    // delete labels
    delete frontCameraLabel;
    delete leftCameraLabel;
    delete rightCameraLabel;
    delete backCameraLabel;

    delete brakeLabel;
    delete gpsLabel;
    delete speedLabel;
    delete throttleLabel;
    delete steeringLabel;
    delete imuLabel;
    

    delete ui;
}


void MainWindow::updateCameraFrames(const cv::Mat& frontFrame, const cv::Mat& leftFrame, const cv::Mat& rightFrame, const cv::Mat& backFrame) {
    displayFrame(frontCameraLabel, frontFrame);
    displayFrame(leftCameraLabel, leftFrame);
    displayFrame(rightCameraLabel, rightFrame);
    displayFrame(backCameraLabel, backFrame);
}

void MainWindow::updateSensorData(const QString& brakeData, const QString& gpsData, const QString& speedData, const QString& throttleData, const QString& steeringData, const QString& imuData) {
    brakeLabel->setText(brakeData);
    gpsLabel->setText("GPS Data (Latitude, Longitude, Height):\n " + gpsData);
    speedLabel->setText(speedData);
    throttleLabel->setText(throttleData);
    steeringLabel->setText(steeringData);
    imuLabel->setText(imuData);

    // Extract latitude and longitude from the GPS data
    QStringList gpsCoordinates = gpsData.split(",");

    if (gpsCoordinates.size() >= 2) {
        bool latOk, lonOk;
        double latitude = gpsCoordinates[0].toDouble(&latOk);
        double longitude = gpsCoordinates[1].toDouble(&lonOk);
        if (latOk && lonOk) {
            //updateMapLocation(latitude, longitude);
        }
    }
}

void MainWindow::displayFrame(QLabel *label, const cv::Mat& frame) {
    if (!frame.empty()) {
        // Convert cv::Mat to QImage
        QImage img((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img.rgbSwapped());
        label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::updateMapLocation(double latitude, double longitude) {
    QString jsCode = QString("var latLng = new google.maps.LatLng(%1, %2); "
                             "var map = new google.maps.Map(document.getElementById('map'), {zoom: 10, center: latLng}); "
                             "var marker = new google.maps.Marker({position: latLng, map: map});")
                             .arg(latitude)
                             .arg(longitude);
    mapView->page()->runJavaScript(jsCode);
}