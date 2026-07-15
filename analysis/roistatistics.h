#ifndef ROISTATISTICS_H
#define ROISTATISTICS_H

#include <QRect>
#include <QVector>

struct RoiStatistics
{
    QRect rect;

    QVector<int> histogram;

    double mean = 0.0;
    double min = 0.0;
    double max = 0.0;
    double standardDeviation = 0.0;

    int pixelCount = 0;

    bool isValid() const
    {
        return !rect.isEmpty() && pixelCount > 0;
    }
};

#endif // ROISTATISTICS_H