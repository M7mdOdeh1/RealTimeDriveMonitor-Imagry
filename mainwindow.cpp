#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Create the main layout
    QWidget *centralWidget = ui->centralwidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Set the window size
    this->setFixedSize(1920, 1080);

    // Create the left layout, which will contain the sensor data and the map
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Create the sensor data layout (top part of the left side)
    QWidget *sensorWidget = new QWidget(this);
    QVBoxLayout *sensorLayout = new QVBoxLayout(sensorWidget);

    // Enhance the sensor layout by adding padding and adjusting font size
    QString sensorStyle = "padding: 10px; font-size: 16px; color: white; background-color: #333333; border-radius: 10px;";
    
    brakeLabel = new QLabel("Brake Data", this);
    brakeLabel->setStyleSheet(sensorStyle);
    
    gpsLabel = new QLabel("GPS Data", this);
    gpsLabel->setStyleSheet(sensorStyle);
    
    speedLabel = new QLabel("Speed Data", this);
    speedLabel->setStyleSheet(sensorStyle);
    
    throttleLabel = new QLabel("Throttle Data", this);
    throttleLabel->setStyleSheet(sensorStyle);
    
    steeringLabel = new QLabel("Steering Data", this);
    steeringLabel->setStyleSheet(sensorStyle);
    
    imuLabel = new QLabel("IMU Data", this);
    imuLabel->setStyleSheet(sensorStyle);

    sensorLayout->addWidget(brakeLabel);
    sensorLayout->addWidget(gpsLabel);
    sensorLayout->addWidget(speedLabel);
    sensorLayout->addWidget(throttleLabel);
    sensorLayout->addWidget(steeringLabel);
    sensorLayout->addWidget(imuLabel);

    // Set a fixed width for the sensorWidget
    sensorWidget->setFixedWidth(600);  
    sensorWidget->setFixedHeight(450);
    sensorWidget->setStyleSheet("border: 1px solid lime");

    // Add sensor data layout to the top part of the left side
    leftLayout->addWidget(sensorWidget);

    // Create the map view (bottom part of the left side)
    mapView = new QWebEngineView(this);
    mapView->setFixedSize(600, 450);  // Increased size of the map view
    mapView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    mapView->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    mapView->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    mapView->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    mapView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    mapView->setUrl(QUrl::fromLocalFile("/home/m7mdodeh/Desktop/imagry_training/qt_projects/RealTimeDriveMonitor/RealTimeDriveMonitor/map.html"));

    // margin for map view
    mapView->setContentsMargins(0, 0, 0, 0);

    // Add the map view to the bottom part of the left side
    leftLayout->addWidget(mapView);

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

    // Set reduced size for each frame label to allow more space for the map
    frontCameraLabel->setFixedSize(550, 450);  
    leftCameraLabel->setFixedSize(550, 450);   
    rightCameraLabel->setFixedSize(550, 450);  
    backCameraLabel->setFixedSize(550, 450);   

    // Add widgets to the camera layout in a 2x2 grid
    cameraLayout->addWidget(frontCameraLabel, 0, 1);
    cameraLayout->addWidget(leftCameraLabel, 0, 0);
    cameraLayout->addWidget(rightCameraLabel, 1, 0);
    cameraLayout->addWidget(backCameraLabel, 1, 1);

    //set spacing 
    cameraLayout->setSpacing(10);

    cameraLayout->setContentsMargins(0, 0, 0, 0);  

    mainLayout->addLayout(leftLayout);  
    mainLayout->addLayout(cameraLayout);  

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
            updateMapLocation(latitude, longitude);
        }
    }
}

void MainWindow::displayFrame(QLabel *label, const cv::Mat& frame) {
    if (!frame.empty()) {
        // Convert cv::Mat to QImage
        QImage img((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img.rgbSwapped());
        //dont keep aspect ratio
        label->setPixmap(pixmap.scaled(label->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
}

void MainWindow::updateMapLocation(double latitude, double longitude) {
    QString jsCode = QString("updateLocation(%1, %2);")
                            .arg(latitude)
                            .arg(longitude);
    mapView->page()->runJavaScript(jsCode);
}
