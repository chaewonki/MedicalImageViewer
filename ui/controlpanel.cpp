#include "ui/controlpanel.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>

ControlPanel::ControlPanel(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    windowLevelButton = new QPushButton("Window/Level", this);
    windowLevelButton->setCheckable(true);

    roiButton = new QPushButton("ROI Select", this);
    roiButton->setCheckable(true);

    magnifierButton = new QPushButton("Magnifier", this);
    magnifierButton->setCheckable(true);

    QPushButton *histogramButton = new QPushButton("Show Histogram", this);

    QPushButton *edgeButton = new QPushButton("Edge Enhancement", this);
    QPushButton *edgeLapButton = new QPushButton("Edge Enhancement-Lap", this);
    QPushButton *sharpenButton = new QPushButton("Sharpen", this);
    QPushButton *contrastButton = new QPushButton("Contrast Enhancement", this);

    QPushButton *flipHButton = new QPushButton("Flip H", this);
    QPushButton *flipVButton = new QPushButton("Flip V", this);
    QPushButton *rotate90Button = new QPushButton("Rotate 90", this);
    QPushButton *rotate180Button = new QPushButton("Rotate 180", this);
    QPushButton *invertButton = new QPushButton("Inversion", this);
    QPushButton *cropButton = new QPushButton("Crop ROI", this);

    QPushButton *detailButton = new QPushButton("Detail", this);
    QPushButton *brightnessButton = new QPushButton("Brightness", this);
    QPushButton *amplifierButton = new QPushButton("Amplifier / Gain", this);
    QPushButton *noiseButton = new QPushButton("Noise Reduction", this);
    QPushButton *dynamicRangeButton = new QPushButton("Dynamic Range", this);

    connect(windowLevelButton, &QPushButton::toggled,
            this, &ControlPanel::windowLevelToggled);
    connect(roiButton, &QPushButton::toggled,
            this, &ControlPanel::roiToggled);
    connect(magnifierButton, &QPushButton::toggled,
            this, &ControlPanel::magnifierToggled);

    connect(histogramButton, &QPushButton::clicked,
            this, &ControlPanel::histogramClicked);

    connect(edgeButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::EdgeDetection);
    });
    connect(edgeLapButton, &QPushButton::clicked, this, [this](){
        emit imageOperationRequested(ImageOperation::EdgeLapDection);
    });

    connect(sharpenButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Sharpen);
    });

    connect(contrastButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::ContrastEnhancement);
    });

    connect(flipHButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::FlipHorizontal);
    });

    connect(flipVButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::FlipVertical);
    });

    connect(rotate90Button, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Rotate90);
    });

    connect(rotate180Button, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Rotate180);
    });

    connect(invertButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Invert);
    });

    connect(cropButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Crop);
    });

    connect(detailButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::DetailEnhancement);
    });

    connect(brightnessButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Brightness);
    });

    connect(amplifierButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::Amplifier);
    });

    connect(noiseButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::NoiseReduction);
    });

    connect(dynamicRangeButton, &QPushButton::clicked, this, [this]() {
        emit imageOperationRequested(ImageOperation::DynamicRange);
    });

    layout->addWidget(new QLabel("Window / Level"));
    layout->addWidget(windowLevelButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Magnifying"));
    layout->addWidget(magnifierButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("ROI"));
    layout->addWidget(roiButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Histogram"));
    layout->addWidget(histogramButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Filters"));
    layout->addWidget(contrastButton);
    layout->addWidget(sharpenButton);
    layout->addWidget(edgeButton);
    layout->addWidget(edgeLapButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Transform"));
    layout->addWidget(flipHButton);
    layout->addWidget(flipVButton);
    layout->addWidget(rotate90Button);
    layout->addWidget(rotate180Button);
    layout->addWidget(invertButton);
    layout->addWidget(cropButton);

    layout->addSpacing(20);
    layout->addWidget(new QLabel("Advanced Image Processing"));
    layout->addWidget(detailButton);
    layout->addWidget(brightnessButton);
    layout->addWidget(amplifierButton);
    layout->addWidget(noiseButton);
    layout->addWidget(dynamicRangeButton);

    layout->addStretch();
}

void ControlPanel::setWindowLevelChecked(bool checked)
{
    QSignalBlocker blocker(windowLevelButton);
    windowLevelButton->setChecked(checked);
}

void ControlPanel::setRoiChecked(bool checked)
{
    QSignalBlocker blocker(roiButton);
    roiButton->setChecked(checked);
}

void ControlPanel::setMagnifierChecked(bool checked)
{
    QSignalBlocker blocker(magnifierButton);
    magnifierButton->setChecked(checked);
}