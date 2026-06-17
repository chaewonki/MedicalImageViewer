#include "mainwindow.h"
#include "imageviewer.h"

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

#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

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

    QLabel *wlLabel = new QLabel("Window Level", this);

    QSlider *windowSlider = new QSlider(Qt::Horizontal, this);
    windowSlider->setRange(1, 4000);
    windowSlider->setValue(400);

    QSlider *levelSlider = new QSlider(Qt::Horizontal, this);
    levelSlider->setRange(-1000, 3000);
    levelSlider->setValue(40);

    QPushButton *histogramButton = new QPushButton("Show Histogram", this);
    QPushButton *edgeButton = new QPushButton("Edge Detection", this);
    QPushButton *sharpenButton = new QPushButton("Sharpen", this);
    QPushButton *contrastButton = new QPushButton("Contrast Enhancement", this);

    layout->addWidget(wlLabel);
    layout->addWidget(new QLabel("Window Width"));
    layout->addWidget(windowSlider);
    layout->addWidget(new QLabel("Window Center"));
    layout->addWidget(levelSlider);

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

    DicomImage dicomImage(fileName.toStdString().c_str());

    if (dicomImage.getStatus() != EIS_Normal) {
        statusBar()->showMessage("Failed to load DICOM");
        return;
    }

    dicomImage.setMinMaxWindow();

    const int width = dicomImage.getWidth();
    const int height = dicomImage.getHeight();

    const uchar *pixelData = static_cast<const uchar*>(
        dicomImage.getOutputData(8)
        );

    if (!pixelData) {
        statusBar()->showMessage("Failed to get pixel data");
        return;
    }

    QImage image(pixelData, width, height, QImage::Format_Grayscale8);

    viewer->setImage(image.copy());

    statusBar()->showMessage("Loaded: " + fileName);
}