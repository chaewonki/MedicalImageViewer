#include "ui/mainwindow.h"
#include "ui/imageviewer.h"

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

#include "dicom/dicomloader.h"
#include "processing/imageprocessor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    viewer(new ImageViewer(this))
{
    setWindowTitle("Mini Medical Image Viewer");
    resize(1200, 800);

    setCentralWidget(viewer);

    createMenu();
    createToolBar();
    createRightPanel();

    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("File");

    QAction *openAction = new QAction("Open Image", this);
    connect(openAction, &QAction::triggered, this, &MainWindow::openDicom);

    fileMenu->addAction(openAction);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");

    QAction *openAction = new QAction("Open DICOM", this);
    QAction *zoomInAction = new QAction("Zoom In", this);
    QAction *zoomOutAction = new QAction("Zoom Out", this);
    QAction *fitAction = new QAction("Fit", this);
    QAction *resetAction = new QAction("Reset", this);

    connect(openAction, &QAction::triggered, this, &MainWindow::openDicom);
    connect(zoomInAction, &QAction::triggered, viewer, &ImageViewer::zoomIn);
    connect(zoomOutAction, &QAction::triggered, viewer, &ImageViewer::zoomOut);
    connect(fitAction, &QAction::triggered, viewer, &ImageViewer::fitToWindow);
    connect(resetAction, &QAction::triggered, viewer, &ImageViewer::resetView);

    toolBar->addAction(openAction);
    toolBar->addAction(zoomInAction);
    toolBar->addAction(zoomOutAction);
    toolBar->addAction(fitAction);
    toolBar->addAction(resetAction);
}

void MainWindow::createRightPanel()
{
    QDockWidget *dock = new QDockWidget("Controls", this);
    QWidget *panel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(panel);

    windowLevelButton = new QPushButton("Window/Level", this);
    windowLevelButton->setCheckable(true);

    connect(windowLevelButton, &QPushButton::clicked,
            this, &MainWindow::toggleWindowLevelMode);

    connect(viewer, &ImageViewer::windowLevelDragged,
            this, &MainWindow::onWindowLevelDragged);

    layout->addWidget(new QLabel("Window / Level"));
    layout->addWidget(windowLevelButton);

    QPushButton *histogramButton = new QPushButton("Show Histogram", this);
    QPushButton *edgeButton = new QPushButton("Edge Detection", this);
    QPushButton *sharpenButton = new QPushButton("Sharpen", this);
    QPushButton *contrastButton = new QPushButton("Contrast Enhancement", this);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Histogram"));
    layout->addWidget(histogramButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Filters"));
    layout->addWidget(edgeButton);
    layout->addWidget(sharpenButton);
    layout->addWidget(contrastButton);

    layout->addStretch();

    panel->setLayout(layout);
    dock->setWidget(panel);

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

    int minValue = static_cast<int>(currentDicom.minValue);
    int maxValue = static_cast<int>(currentDicom.maxValue);

    if (maxValue <= minValue)
        maxValue = minValue + 1;

    updateDisplayImage(true);
    updateWindowLevelStatus();

    statusBar()->showMessage(
        QString("Loaded: %1 | WW: %2 WL: %3")
            .arg(fileName)
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

void MainWindow::toggleWindowLevelMode()
{
    windowLevelMode = windowLevelButton->isChecked();
    viewer->setWindowLevelMode(windowLevelMode);

    if (windowLevelMode) {
        statusBar()->showMessage("Window/Level mode ON - drag left/right for WW, up/down for WL");
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

void MainWindow::onWindowSliderChanged(int value)
{
    if (!currentDicom.isValid())
        return;

    currentDicom.windowWidth = value;

    updateDisplayImage(false);
    updateWindowLevelStatus();
}

void MainWindow::onLevelSliderChanged(int value)
{
    if (!currentDicom.isValid())
        return;

    currentDicom.windowCenter = value;

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