#include "ui/mainwindow.h"

#include "analysis/roianalyzer.h"
#include "ui/controlpanel.h"
#include "ui/imageviewer.h"
#include "ui/roihistogramdialog.h"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QRect>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    viewer(new ImageViewer(this))
{
    setWindowTitle("Medical Image Viewer");
    resize(1200, 800);

    setCentralWidget(viewer);

    createToolBar();
    createRightPanel();

    statusBar()->showMessage("Ready");
}

void MainWindow::openDicom()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open DICOM",
        "",
        "DICOM Files (*.dcm);;All Files (*)"
        );

    if (fileName.isEmpty())
        return;

    QString errorMessage;

    if (!document.load(fileName, errorMessage)) {
        statusBar()->showMessage(errorMessage);
        return;
    }

    clearRoiState();

    viewer->setImage(document.displayImage(), true);
    updateWindowLevelStatus();

    const DicomImageData& dicom = document.dicom();

    statusBar()->showMessage(
        QString("Loaded: %1 | WW: %2 WL: %3")
            .arg(fileName)
            .arg(dicom.windowWidth, 0, 'f', 0)
            .arg(dicom.windowCenter, 0, 'f', 0)
        );
}

void MainWindow::resetImage()
{
    if (!document.isLoaded())
        return;

    document.reset();
    clearRoiState();

    viewer->setImage(document.displayImage(), true);
    updateWindowLevelStatus();
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar("Main Toolbar");

    QAction* openAction = new QAction("Open DICOM", this);
    QAction* fitAction = new QAction("Fit", this);
    QAction* actualSizeAction = new QAction("Actual Size", this);
    QAction* resetAction = new QAction("Reset", this);

    connect(openAction, &QAction::triggered, this, &MainWindow::openDicom);
    connect(fitAction, &QAction::triggered, viewer, &ImageViewer::fitToWindow);
    connect(actualSizeAction, &QAction::triggered,
            viewer, &ImageViewer::actualSize);
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetImage);

    toolBar->addAction(openAction);
    toolBar->addAction(fitAction);
    toolBar->addAction(actualSizeAction);
    toolBar->addAction(resetAction);
}

void MainWindow::createRightPanel()
{
    QDockWidget* dock = new QDockWidget("Controls", this);

    controlPanel = new ControlPanel(this);

    connect(viewer, &ImageViewer::windowLevelDragged,
            this, &MainWindow::onWindowLevelDragged);

    connect(viewer, &ImageViewer::roiSelected,
            this, &MainWindow::onRoiSelected);

    connect(controlPanel, &ControlPanel::windowLevelToggled,
            this, &MainWindow::setWindowLevelMode);

    connect(controlPanel, &ControlPanel::roiToggled,
            this, &MainWindow::setRoiMode);

    connect(controlPanel, &ControlPanel::magnifierToggled,
            this, &MainWindow::setMagnifierMode);

    connect(controlPanel, &ControlPanel::histogramClicked,
            this, &MainWindow::showRoiHistogram);

    connect(controlPanel, &ControlPanel::imageOperationRequested,
            this, &MainWindow::applyImageOperation);

    dock->setWidget(controlPanel);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::setInteractionMode(
    InteractionMode mode
    )
{
    interactionMode = mode;

    const bool windowLevelEnabled =
        mode == InteractionMode::WindowLevel;

    const bool roiEnabled =
        mode == InteractionMode::Roi;

    const bool magnifierEnabled =
        mode == InteractionMode::Magnifier;

    controlPanel->setWindowLevelChecked(
        windowLevelEnabled
        );
    controlPanel->setRoiChecked(roiEnabled);
    controlPanel->setMagnifierChecked(
        magnifierEnabled
        );

    viewer->setWindowLevelMode(
        windowLevelEnabled
        );
    viewer->setRoiMode(roiEnabled);
    viewer->setMagnifierMode(
        magnifierEnabled
        );

    switch (mode) {
    case InteractionMode::WindowLevel:
        statusBar()->showMessage(
            "Window/Level mode ON - "
            "right-drag left/right for WW, "
            "up/down for WL"
            );
        break;

    case InteractionMode::Roi:
        statusBar()->showMessage(
            "ROI mode ON - "
            "left-drag to select a rectangular ROI"
            );
        break;

    case InteractionMode::Magnifier:
        statusBar()->showMessage(
            "Magnifier mode ON - "
            "move mouse to magnify, "
            "wheel to change scale"
            );
        break;

    case InteractionMode::None:
        updateWindowLevelStatus();
        break;
    }
}

void MainWindow::setWindowLevelMode(bool enabled)
{
    setInteractionMode(
        enabled
            ? InteractionMode::WindowLevel
            : InteractionMode::None
        );
}

void MainWindow::onWindowLevelDragged(
    int deltaX,
    int deltaY
    )
{
    if (!document.isLoaded())
        return;

    document.adjustWindowLevel(deltaX, deltaY);

    viewer->setImage(document.displayImage(), false);
    updateWindowLevelStatus();
}

void MainWindow::updateWindowLevelStatus()
{
    if (!document.isLoaded()) {
        statusBar()->showMessage("No image loaded");
        return;
    }

    const DicomImageData& dicom = document.dicom();

    statusBar()->showMessage(
        QString("Window Width: %1 | Window Level: %2")
            .arg(dicom.windowWidth, 0, 'f', 0)
            .arg(dicom.windowCenter, 0, 'f', 0)
        );
}

void MainWindow::setRoiMode(bool enabled)
{
    if (!enabled)
        clearRoiState();

    setInteractionMode(
        enabled
            ? InteractionMode::Roi
            : InteractionMode::None
        );
}

void MainWindow::onRoiSelected(const QRect& roiRect)
{
    if (!document.isLoaded())
        return;

    currentRoiStatistics =
        RoiAnalyzer::calculate(document.dicom(), roiRect);

    if (!currentRoiStatistics.isValid()) {
        clearRoiState();
        statusBar()->showMessage("Failed to analyze ROI");
        return;
    }

    statusBar()->showMessage(
        QString(
            "ROI: x=%1 y=%2 w=%3 h=%4 | "
            "Mean: %5 | Min: %6 | Max: %7 | SD: %8"
            )
            .arg(currentRoiStatistics.rect.x())
            .arg(currentRoiStatistics.rect.y())
            .arg(currentRoiStatistics.rect.width())
            .arg(currentRoiStatistics.rect.height())
            .arg(currentRoiStatistics.mean, 0, 'f', 2)
            .arg(currentRoiStatistics.min, 0, 'f', 2)
            .arg(currentRoiStatistics.max, 0, 'f', 2)
            .arg(
                currentRoiStatistics.standardDeviation,
                0,
                'f',
                2
                )
        );
}

void MainWindow::clearRoiState()
{
    currentRoiStatistics = RoiStatistics{};

    viewer->clearRoi();
}

void MainWindow::showRoiHistogram()
{
    if (!currentRoiStatistics.isValid()) {
        statusBar()->showMessage("No ROI selected");
        return;
    }

    const DicomImageData& dicom = document.dicom();

    RoiHistogramDialog dialog(
        currentRoiStatistics,
        dicom.minValue,
        dicom.maxValue,
        this
        );

    dialog.exec();
}

void MainWindow::setMagnifierMode(bool enabled)
{
    setInteractionMode(
        enabled
            ? InteractionMode::Magnifier
            : InteractionMode::None
        );
}

void MainWindow::applyImageOperation(
    ImageOperation operation
    )
{
    if (!document.isLoaded())
        return;

    if (operation == ImageOperation::Crop) {
        if (!currentRoiStatistics.isValid()) {
            statusBar()->showMessage(
                "Select an ROI before cropping"
                );
            return;
        }

        if (!document.crop(currentRoiStatistics.rect)) {
            statusBar()->showMessage("Crop failed");
            return;
        }

        clearRoiState();

        viewer->setImage(
            document.displayImage(),
            true
            );

        statusBar()->showMessage("Crop applied");
        return;
    }

    document.applyOperation(operation);

    const bool resetView =
        operation == ImageOperation::Rotate90 ||
        operation == ImageOperation::Rotate180;

    viewer->setImage(
        document.displayImage(),
        resetView
        );

    statusBar()->showMessage("Image operation applied");
}