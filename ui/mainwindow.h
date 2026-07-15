#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysis/roistatistics.h"
#include "document/imagedocument.h"
#include "processing/imageoperation.h"

#include <QMainWindow>
#include <QRect>

class ImageViewer;
class ControlPanel;

enum class InteractionMode
{
    None,
    WindowLevel,
    Roi,
    Magnifier
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    // Image loading and display
    void openDicom();
    void resetImage();

private:
    // UI
    ImageViewer* viewer = nullptr;
    ControlPanel* controlPanel = nullptr;

    // Image data
    ImageDocument document;

    InteractionMode interactionMode = InteractionMode::None;
    // ROI state
    RoiStatistics currentRoiStatistics;

    // UI construction
    void createToolBar();
    void createRightPanel();

    void setInteractionMode(InteractionMode mode);

    // Window / Level
    void setWindowLevelMode(bool enabled);
    void onWindowLevelDragged(int deltaX, int deltaY);
    void updateWindowLevelStatus();

    // ROI
    void setRoiMode(bool enabled);
    void onRoiSelected(const QRect& roiRect);
    void clearRoiState();
    void showRoiHistogram();

    // Magnifier
    void setMagnifierMode(bool enabled);

    // Image processing
    void applyImageOperation(ImageOperation operation);
};

#endif