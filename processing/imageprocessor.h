#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "dicom/dicomimagedata.h"
#include <QImage>

class ImageProcessor
{
public:
    static QImage applyWindowLevel(
        const DicomImageData& data,
        double windowWidth,
        double windowCenter
        );
};

#endif // IMAGEPROCESSOR_H
