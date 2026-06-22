#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "processing/imageoperation.h"

class QPushButton;

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);

    void setWindowLevelChecked(bool checked);
    void setRoiChecked(bool checked);
    void setMagnifierChecked(bool checked);

signals:
    void windowLevelToggled(bool enabled);
    void roiToggled(bool enabled);
    void magnifierToggled(bool enabled);

    void histogramClicked();

    void imageOperationRequested(ImageOperation operation);

private:
    QPushButton *windowLevelButton = nullptr;
    QPushButton *roiButton = nullptr;
    QPushButton *magnifierButton = nullptr;
};

#endif