#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dicom/dicomimagedata.h"
#include "processing/imageoperation.h"
#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QRect>
#include <QVector>

class ImageViewer;
class ControlPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openDicom();
    void resetImage();

private:
    ImageViewer *viewer;
    ControlPanel *controlPanel = nullptr;

    DicomImageData currentDicom;
    DicomImageData originalDicom;

    QImage currentDisplayImage;

    bool windowLevelMode = false;
    bool roiMode = false;
    QRect currentRoiRect;
    bool hasRoiStats = false;
    QVector<int> currentRoiHistogram;
    double currentRoiMean = 0.0;
    double currentRoiMin = 0.0;
    double currentRoiMax = 0.0;
    double currentRoiStdDev = 0.0;
    int currentRoiCount = 0;
    bool magnifierMode = false;

    void createToolBar();
    void createRightPanel();

    void updateDisplayImage();
    void updateDisplayImage(bool resetView);

    void updateWindowLevelStatus();
    void onWindowLevelDragged(int deltaX, int deltaY);


    void onRoiSelected(const QRect &roiRect);
    void calculateRoiStatistics(const QRect &roiRect);
    void showRoiHistogram();
    void setWindowLevelMode(bool enabled);
    void setRoiMode(bool enabled);
    void clearRoiState();

    void setMagnifierMode(bool enabled);

    void applyImageOperation(ImageOperation operation);
};

#endif