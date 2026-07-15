#include "analysis/roianalyzer.h"

#include <algorithm>
#include <cmath>
#include <limits>

RoiStatistics RoiAnalyzer::calculate(
    const DicomImageData& imageData,
    const QRect& roiRect,
    int histogramBinCount
)
{
    RoiStatistics result;

    if (!imageData.isValid())
        return result;

    if (histogramBinCount <= 0)
        return result;

    const QRect imageRect(
        0,
        0,
        imageData.width,
        imageData.height
        );

    const QRect validRoi = roiRect.intersected(imageRect);

    if (validRoi.isEmpty())
        return result;

    result.rect = validRoi;
    result.histogram.resize(histogramBinCount);
    result.histogram.fill(0);

    double sum = 0.0;
    double sumSquared = 0.0;

    double roiMin = std::numeric_limits<double>::max();
    double roiMax = std::numeric_limits<double>::lowest();

    const double imageMin = imageData.minValue;
    const double imageMax = imageData.maxValue;
    const double imageRange = imageMax - imageMin;

    for (int y = validRoi.top(); y <= validRoi.bottom(); ++y) {
        for (int x = validRoi.left(); x <= validRoi.right(); ++x) {
            const int index = y * imageData.width + x;
            const double value = imageData.pixels[index];

            sum += value;
            sumSquared += value * value;
            ++result.pixelCount;

            roiMin = std::min(roiMin, value);
            roiMax = std::max(roiMax, value);

            int histogramIndex = 0;

            if (imageRange > 0.0) {
                const double normalized =
                    (value - imageMin) / imageRange;

                histogramIndex = static_cast<int>(
                    normalized * histogramBinCount
                    );
            }

            histogramIndex = std::clamp(
                histogramIndex,
                0,
                histogramBinCount - 1
                );

            ++result.histogram[histogramIndex];
        }
    }

    if (result.pixelCount <= 0)
        return RoiStatistics{};

    result.mean = sum / result.pixelCount;
    result.min = roiMin;
    result.max = roiMax;

    double variance =
        (sumSquared / result.pixelCount) -
        (result.mean * result.mean);

    if (variance < 0.0)
        variance = 0.0;

    result.standardDeviation = std::sqrt(variance);

    return result;

}