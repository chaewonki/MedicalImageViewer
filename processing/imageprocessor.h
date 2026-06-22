#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "dicom/dicomimagedata.h"
#include "processing/imageoperation.h"

#include <QImage>

class ImageProcessor
{
public:
    static QImage applyWindowLevel(
        const DicomImageData& data,
        double windowWidth,
        double windowCenter
        );

    static QImage apply(const QImage& image, ImageOperation operation);

    static QImage edgeEnhancement(const QImage& image);
    static QImage edgeEnhancementLap(const QImage& image);
    static QImage sharpen(const QImage& image);
    static QImage contrastEnhancement(const QImage& image);

    static QImage flipHorizontal(const QImage& image);
    static QImage flipVertical(const QImage& image);
    static QImage rotate90(const QImage& image);
    static QImage rotate180(const QImage& image);
    static QImage invert(const QImage& image);

    static QImage detailEnhancement(const QImage& image);
    static QImage brightness(const QImage& image, int value);
    static QImage contrast(const QImage& image, double alpha);
    static QImage amplifier(const QImage& image, double gain);
    static QImage noiseReduction(const QImage& image);
    static QImage dynamicRange(const QImage& image);
};

#endif