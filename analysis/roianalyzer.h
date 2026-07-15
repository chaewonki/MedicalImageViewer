#ifndef ROIANALYZER_H
#define ROIANALYZER_H

#include "analysis/roistatistics.h"
#include "dicom/dicomimagedata.h"

#include <QRect>

class RoiAnalyzer
{
public:
    static RoiStatistics calculate(
        const DicomImageData& imageData,
        const QRect& roiRect,
        int histogramBinCount = 256
        );
};

#endif