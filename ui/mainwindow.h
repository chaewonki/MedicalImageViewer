#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dicom/dicomimagedata.h"
#include <QMainWindow>
#include <QSlider>
#include <QPushButton>

class ImageViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openDicom();

private:
    ImageViewer *viewer;

    DicomImageData currentDicom;
    QImage currentDisplayImage;

    QPushButton *windowLevelButton = nullptr;

    bool windowLevelMode = false;

    void createMenu();
    void createToolBar();
    void createRightPanel();

    void updateDisplayImage();
    void updateDisplayImage(bool resetView);
    void updateWindowLevelStatus();

    void toggleWindowLevelMode();
    void onWindowLevelDragged(int deltaX, int deltaY);
    void onWindowSliderChanged(int value);
    void onLevelSliderChanged(int value);
};

#endif