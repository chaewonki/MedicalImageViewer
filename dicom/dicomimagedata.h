#ifndef DICOMIMAGEDATA_H
#define DICOMIMAGEDATA_H

#include <QVector>
#include <QString>

struct DicomImageData{
    int width = 0;
    int height = 0;

    QVector<double> pixels;

    double minValue = 0.0;
    double maxValue = 0.0;

    double windowWidth = 1.0;
    double windowCenter = 0.0;

    bool inverted = false; // MONOCHROME1 대응

    bool isValid() const{
        return width > 0 && height > 0 && pixels.size () == width * height;
    }
};

#endif // DICOMIMAGEDATA_H
