#ifndef ROIHISTOGRAMDIALOG_H
#define ROIHISTOGRAMDIALOG_H

#include "analysis/roistatistics.h"

#include <QDialog>

class RoiHistogramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoiHistogramDialog(
        const RoiStatistics& statistics,
        double imageMinValue,
        double imageMaxValue,
        QWidget* parent = nullptr
        );
};

#endif