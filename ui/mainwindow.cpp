#include "ui/mainwindow.h"
#include "ui/imageviewer.h"
#include "ui/controlpanel.h"

#include <QFileDialog>
#include <QToolBar>
#include <QAction>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QImage>
#include <QRect>
#include <QDialog>
#include <QTableWidget>
#include <QHeaderView>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>

#include <limits>
#include <cmath>

#include "dicom/dicomloader.h"
#include "processing/imageprocessor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    viewer(new ImageViewer(this))
{
    setWindowTitle("Mini Medical Image Viewer");
    resize(1200, 800);

    setCentralWidget(viewer);

    createToolBar();
    createRightPanel();

    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
}


void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");

    QAction *openAction = new QAction("Open DICOM", this);
    QAction *fitAction = new QAction("Fit", this);
    QAction *actualSizeAction = new QAction("Actual Size", this);
    QAction *resetAction = new QAction("Reset", this);

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
    QDockWidget *dock = new QDockWidget("Controls", this);

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


void MainWindow::openDicom()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open DICOM",
        "",
        "DICOM Files (*.dcm);;All Files (*)"
        );

    if (fileName.isEmpty())
        return;

    QString errorMessage;

    if (!DicomLoader::load(fileName, currentDicom, errorMessage)) {
        statusBar()->showMessage(errorMessage);
        return;
    }

    originalDicom = currentDicom;

    updateDisplayImage(true);
    updateWindowLevelStatus();

    statusBar()->showMessage(
        QString("Loaded: %1 | WW: %2 WL: %3")
            .arg(fileName)
            .arg(currentDicom.windowWidth, 0, 'f', 0)
            .arg(currentDicom.windowCenter, 0, 'f', 0)
        );
}

void MainWindow::resetImage(){
    if (!originalDicom.isValid())
        return;

    currentDicom = originalDicom;

    currentRoiRect = QRect();
    currentRoiHistogram.clear();
    currentRoiMean = 0.0;
    hasRoiStats = false;

    updateDisplayImage(true);
    updateWindowLevelStatus();

    statusBar()->showMessage(
        QString("Reset | WW: %1 WL: %2")
            .arg(currentDicom.windowWidth, 0, 'f', 0)
            .arg(currentDicom.windowCenter, 0, 'f', 0)
        );
}


void MainWindow::updateDisplayImage()
{
    updateDisplayImage(false);
}

void MainWindow::updateDisplayImage(bool resetView)
{
    if (!currentDicom.isValid())
        return;

    currentDisplayImage = ImageProcessor::applyWindowLevel(
        currentDicom,
        currentDicom.windowWidth,
        currentDicom.windowCenter
        );

    viewer->setImage(currentDisplayImage, resetView);
}


void MainWindow::setWindowLevelMode(bool enabled)
{
    windowLevelMode = enabled;

    if (enabled) {
        roiMode = false;
        magnifierMode = false;

        controlPanel->setRoiChecked(false);
        controlPanel->setMagnifierChecked(false);

        viewer->setRoiMode(false);
        viewer->setMagnifierMode(false);
    }

    controlPanel->setWindowLevelChecked(enabled);
    viewer->setWindowLevelMode(enabled);

    if (enabled) {
        statusBar()->showMessage("Window/Level mode ON - right-drag left/right for WW, up/down for WL");
    } else {
        updateWindowLevelStatus();
    }
}

void MainWindow::onWindowLevelDragged(int deltaX, int deltaY)
{
    if (!currentDicom.isValid())
        return;

    const double widthStep = 1.0;
    const double levelStep = 1.0;

    currentDicom.windowWidth += deltaX * widthStep;
    currentDicom.windowCenter -= deltaY * levelStep;

    if (currentDicom.windowWidth < 1.0)
        currentDicom.windowWidth = 1.0;

    if (currentDicom.windowCenter < currentDicom.minValue)
        currentDicom.windowCenter = currentDicom.minValue;

    if (currentDicom.windowCenter > currentDicom.maxValue)
        currentDicom.windowCenter = currentDicom.maxValue;

    updateDisplayImage(false);
    updateWindowLevelStatus();
}

void MainWindow::updateWindowLevelStatus()
{
    statusBar()->showMessage(
        QString("Window Width: %1 | Window Level: %2")
            .arg(currentDicom.windowWidth, 0, 'f', 0)
            .arg(currentDicom.windowCenter, 0, 'f', 0)
        );
}


void MainWindow::setRoiMode(bool enabled)
{
    roiMode = enabled;

    if (enabled) {
        windowLevelMode = false;
        magnifierMode = false;

        controlPanel->setWindowLevelChecked(false);
        controlPanel->setMagnifierChecked(false);

        viewer->setWindowLevelMode(false);
        viewer->setMagnifierMode(false);
    }
    else{
        viewer->clearRoi();
        clearRoiState();
    }

    controlPanel->setRoiChecked(enabled);
    viewer->setRoiMode(enabled);

    if (enabled) {
        statusBar()->showMessage("ROI mode ON - left-drag to select a rectangular ROI");
    } else {
        updateWindowLevelStatus();
    }
}

void MainWindow::clearRoiState()
{
    currentRoiRect = QRect();
    currentRoiHistogram.clear();

    currentRoiMean = 0.0;
    currentRoiMin = 0.0;
    currentRoiMax = 0.0;
    currentRoiStdDev = 0.0;
    currentRoiCount = 0;
    hasRoiStats = false;

    viewer->clearRoi();
}

void MainWindow::onRoiSelected(const QRect &roiRect)
{
    if (!currentDicom.isValid())
        return;

    currentRoiRect = roiRect;
    calculateRoiStatistics(roiRect);

    statusBar()->showMessage(
        QString("ROI: x=%1 y=%2 w=%3 h=%4 | Mean: %5 | Min: %6 | Max: %7 | SD: %8")
            .arg(roiRect.x())
            .arg(roiRect.y())
            .arg(roiRect.width())
            .arg(roiRect.height())
            .arg(currentRoiMean, 0, 'f', 2)
            .arg(currentRoiMin, 0, 'f', 2)
            .arg(currentRoiMax, 0, 'f', 2)
            .arg(currentRoiStdDev, 0, 'f', 2)
        );
}

void MainWindow::calculateRoiStatistics(const QRect &roiRect)
{
    const int binCount = 256;

    currentRoiHistogram.clear();
    currentRoiHistogram.resize(binCount);
    currentRoiHistogram.fill(0);

    currentRoiMean = 0.0;
    currentRoiMin = 0.0;
    currentRoiMax = 0.0;
    currentRoiStdDev = 0.0;
    hasRoiStats = false;

    const double minValue = currentDicom.minValue;
    const double maxValue = currentDicom.maxValue;
    const double range = maxValue - minValue;

    if (range <= 0.0)
        return;

    QRect validRoi = roiRect.intersected(
        QRect(0, 0, currentDicom.width, currentDicom.height)
        );

    if (validRoi.isEmpty())
        return;

    double sum = 0.0;
    double sumSquared = 0.0;
    int count = 0;

    double roiMin = std::numeric_limits<double>::max();
    double roiMax = std::numeric_limits<double>::lowest();

    for (int y = validRoi.top(); y <= validRoi.bottom(); ++y) {
        for (int x = validRoi.left(); x <= validRoi.right(); ++x) {
            const int index = y * currentDicom.width + x;
            const double value = currentDicom.pixels[index];

            sum += value;
            sumSquared += value * value;
            ++count;

            if (value < roiMin)
                roiMin = value;

            if (value > roiMax)
                roiMax = value;

            int bin = static_cast<int>(
                ((value - minValue) / range) * binCount
                );

            if (bin < 0)
                bin = 0;

            if (bin >= binCount)
                bin = binCount - 1;

            currentRoiHistogram[bin]++;
        }
    }

    if (count <= 0)
        return;

    currentRoiMean = sum / count;
    currentRoiMin = roiMin;
    currentRoiMax = roiMax;

    double variance =
        (sumSquared / count) -
        (currentRoiMean * currentRoiMean);

    if (variance < 0.0)
        variance = 0.0;

    currentRoiStdDev = std::sqrt(variance);
    hasRoiStats = true;
    currentRoiCount = count;
}

void MainWindow::showRoiHistogram()
{
    if (!hasRoiStats) {
        statusBar()->showMessage("No ROI selected");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("ROI Histogram");
    dialog.resize(500, 600);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *summaryLabel = new QLabel(
        QString("ROI: x=%1 y=%2 w=%3 h=%4\n"
                "Count: %5\n"
                "Mean: %6\n"
                "Min: %7\n"
                "Max: %8\n"
                "StdDev: %9")
            .arg(currentRoiRect.x())
            .arg(currentRoiRect.y())
            .arg(currentRoiRect.width())
            .arg(currentRoiRect.height())
            .arg(currentRoiCount)
            .arg(currentRoiMean, 0, 'f', 2)
            .arg(currentRoiMin, 0, 'f', 2)
            .arg(currentRoiMax, 0, 'f', 2)
            .arg(currentRoiStdDev, 0, 'f', 2),
        &dialog
        );

    QTableWidget *table = new QTableWidget(&dialog);
    table->setColumnCount(2);
    table->setRowCount(currentRoiHistogram.size());
    table->setHorizontalHeaderLabels(QStringList() << "Value Range" << "Count");

    const double minValue = currentDicom.minValue;
    const double maxValue = currentDicom.maxValue;
    const double range = maxValue - minValue;
    const int binCount = currentRoiHistogram.size();

    QLineSeries *series = new QLineSeries();

    for (int i = 0; i < binCount; ++i) {
        double binCenter = minValue + range * (i + 0.5) / binCount;
        series->append(binCenter, currentRoiHistogram[i]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("ROI Histogram");
    chart->legend()->hide();

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Pixel Value");
    axisX->setRange(minValue, maxValue);
    axisX->setLabelFormat("%.0f");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Count");
    axisY->setLabelFormat("%d");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart, &dialog);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(260);

    for (int i = 0; i < binCount; ++i) {
        double binStart = minValue + (range * i / binCount);
        double binEnd = minValue + (range * (i + 1) / binCount);

        table->setItem(
            i,
            0,
            new QTableWidgetItem(
                QString("%1 ~ %2")
                    .arg(static_cast<int>(binStart))
                    .arg(static_cast<int>(binEnd))
                )
            );

        table->setItem(
            i,
            1,
            new QTableWidgetItem(QString::number(currentRoiHistogram[i]))
            );
    }

    table->horizontalHeader()->setStretchLastSection(true);

    layout->addWidget(summaryLabel);
    layout->addWidget(chartView);
    layout->addWidget(table);

    dialog.exec();
}


void MainWindow::setMagnifierMode(bool enabled)
{
    magnifierMode = enabled;

    if (enabled) {
        windowLevelMode = false;
        roiMode = false;

        controlPanel->setWindowLevelChecked(false);
        controlPanel->setRoiChecked(false);

        viewer->setWindowLevelMode(false);
        viewer->setRoiMode(false);
    }

    controlPanel->setMagnifierChecked(enabled);
    viewer->setMagnifierMode(enabled);

    if (enabled) {
        statusBar()->showMessage("Magnifier mode ON - move mouse to magnify, wheel to change scale");
    } else {
        updateWindowLevelStatus();
    }
}

void MainWindow::applyImageOperation(ImageOperation operation)
{
    if (currentDisplayImage.isNull())
        return;

    if (operation == ImageOperation::Crop) {
        if (currentRoiRect.isNull())
            return;

        QRect validRect = currentRoiRect.intersected(currentDisplayImage.rect());
        currentDisplayImage = currentDisplayImage.copy(validRect);

        viewer->setImage(currentDisplayImage, true);
        statusBar()->showMessage("Crop applied");
        return;
    }

    currentDisplayImage = ImageProcessor::apply(currentDisplayImage, operation);

    const bool resetView =
        operation == ImageOperation::Rotate90 ||
        operation == ImageOperation::Rotate180;

    viewer->setImage(currentDisplayImage, resetView);
    statusBar()->showMessage("Image operation applied");
}