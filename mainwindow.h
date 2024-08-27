#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "include/RealTimeDriverMonitor.h"
#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "DataManager.h"
#include "ClockSystem.h"
#include <QWebEngineView>
#include <QUrl>
#include <QWebEngineProfile>
#include <QWebEngineSettings>




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void updateCameraFrames(const cv::Mat& frontFrame, const cv::Mat& leftFrame, const cv::Mat& rightFrame, const cv::Mat& backFrame);
    void updateSensorData(const QString& brakeData, const QString& gpsData, const QString& speedData, const QString& throttleData, const QString& steeringData, const QString& imuData);

private:
    Ui::MainWindow *ui;
    RealTimeDriverMonitor *driverMonitor;

    QLabel *frontCameraLabel;
    QLabel *leftCameraLabel;
    QLabel *rightCameraLabel;
    QLabel *backCameraLabel;

    QLabel *brakeLabel;
    QLabel *gpsLabel;
    QLabel *speedLabel;
    QLabel *throttleLabel;
    QLabel *steeringLabel;
    QLabel *imuLabel;

    QWebEngineView *mapView;  


    void displayFrame(QLabel *label, const cv::Mat& frame);
    void updateMapLocation(double latitude, double longitude);
};

#endif // MAINWINDOW_H
