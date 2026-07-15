#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include "dicom/dicomimagedata.h"
#include "processing/imageoperation.h"

#include <QImage>
#include <QRect>
#include <QString>

class ImageDocument
{
public:
    bool load(const QString& filePath, QString& errorMessage);
    void reset();

    bool isLoaded() const;

    const DicomImageData& dicom() const;
    const QImage& displayImage() const;

    void adjustWindowLevel(int deltaX, int deltaY);

    void applyOperation(ImageOperation operation);
    bool crop(const QRect& rect);

private:
    void rebuildDisplayImage();

    DicomImageData originalDicom;
    DicomImageData currentDicom;
    QImage currentDisplayImage;
};

#endif