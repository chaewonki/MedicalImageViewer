#ifndef IMAGEOPERATION_H
#define IMAGEOPERATION_H

enum class ImageOperation
{
    EdgeDetection,
    EdgeLapDetection,
    Sharpen,
    ContrastEnhancement,

    FlipHorizontal,
    FlipVertical,
    Rotate90,
    Rotate180,
    Invert,
    Crop,

    DetailEnhancement,
    Brightness,
    Amplifier,
    NoiseReduction,
    DynamicRange
};

#endif // IMAGEOPERATION_H
