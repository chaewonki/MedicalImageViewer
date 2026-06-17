#include "imageprocessor.h"
#include <QtGlobal>

QImage ImageProcessor::applyWindowLevel(
    const DicomImageData& data,
    double windowWidth,
    double windowCenter
    )
{
    QImage image(data.width, data.height, QImage::Format_Grayscale8);

    if (!data.isValid() || windowWidth <= 0.0)
        return image;

    const double minValue = windowCenter - windowWidth / 2.0;
    const double maxValue = windowCenter + windowWidth / 2.0;

    for (int y = 0; y < data.height; ++y) {
        uchar* line = image.scanLine(y);

        for (int x = 0; x < data.width; ++x) {
            int index = y * data.width + x;
            double value = data.pixels[index];

            double normalized = (value - minValue) / (maxValue - minValue);
            normalized = qBound(0.0, normalized, 1.0);

            uchar gray = static_cast<uchar>(normalized * 255.0);

            if (data.inverted)
                gray = 255 - gray;

            line[x] = gray;
        }
    }

    return image;
}