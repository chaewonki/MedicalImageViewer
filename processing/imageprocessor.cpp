#include "imageprocessor.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <QtGlobal>
#include <QTransform>

namespace{

cv::Mat qImageToMat(const QImage& image)
{
    QImage gray = image.convertToFormat(QImage::Format_Grayscale8);

    return cv::Mat(
               gray.height(),
               gray.width(),
               CV_8UC1,
               const_cast<uchar*>(gray.bits()),
               gray.bytesPerLine()
               ).clone();
}

QImage matToQImage(const cv::Mat& mat)
{
    QImage image(
        mat.data,
        mat.cols,
        mat.rows,
        mat.step,
        QImage::Format_Grayscale8
        );

    return image.copy();
}

int clamp(int value)
{
    return qBound(0, value, 255);
}

}

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


QImage ImageProcessor::contrastEnhancement(const QImage& image)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat dst;

    src.convertTo(dst, -1, 1.3, 0);

    return matToQImage(dst);
}

QImage ImageProcessor::sharpen(const QImage& image)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat dst;

    cv::Mat kernel = (cv::Mat_<float>(3,3) <<
                          0,-1, 0,
                      -1, 5,-1,
                      0,-1, 0);

    cv::filter2D(src, dst, -1, kernel);

    return matToQImage(dst);
}

QImage ImageProcessor::edgeEnhancement(const QImage& image)
{
    cv::Mat src = qImageToMat(image);

    cv::Mat blurred;
    cv::Mat dst;

    cv::GaussianBlur(src, blurred, cv::Size(0, 0), 1.5);

    cv::addWeighted(
        src,      1.6,
        blurred, -0.6,
        0,
        dst
        );

    return matToQImage(dst);
}

QImage ImageProcessor::edgeEnhancementLap(const QImage& image)
{
    cv::Mat src = qImageToMat(image);

    cv::Mat laplacian;
    cv::Laplacian(src, laplacian, CV_16S, 3);

    cv::Mat absLaplacian;
    cv::convertScaleAbs(laplacian, absLaplacian);

    cv::Mat dst;
    cv::addWeighted(src, 1.0, absLaplacian, 0.3, 0, dst);

    return matToQImage(dst);
}


QImage ImageProcessor::flipHorizontal(const QImage& image)
{
    return image.flipped(Qt::Horizontal);
}

QImage ImageProcessor::flipVertical(const QImage& image)
{
    return image.flipped(Qt::Vertical);
}

QImage ImageProcessor::rotate90(const QImage& image)
{
    QTransform transform;
    transform.rotate(90);
    return image.transformed(transform);
}

QImage ImageProcessor::rotate180(const QImage& image)
{
    QTransform transform;
    transform.rotate(180);
    return image.transformed(transform);
}

QImage ImageProcessor::invert(const QImage& image)
{
    QImage result = image.convertToFormat(QImage::Format_Grayscale8);
    result.invertPixels();
    return result;
}

QImage ImageProcessor::detailEnhancement(const QImage& image)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat blurred;
    cv::Mat dst;

    cv::GaussianBlur(src, blurred, cv::Size(0, 0), 2.0);
    cv::addWeighted(src, 1.5, blurred, -0.5, 0, dst);

    return matToQImage(dst);
}


QImage ImageProcessor::brightness(const QImage& image, int value)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat dst;

    src.convertTo(dst, -1, 1.0, value);

    return matToQImage(dst);
}

QImage ImageProcessor::amplifier(const QImage& image, double gain)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat dst;

    src.convertTo(dst, -1, gain, 0);

    return matToQImage(dst);
}

QImage ImageProcessor::noiseReduction(const QImage& image)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat dst;

    cv::GaussianBlur(src, dst, cv::Size(3, 3), 0);

    return matToQImage(dst);
}

QImage ImageProcessor::dynamicRange(const QImage& image)
{
    cv::Mat src = qImageToMat(image);
    cv::Mat dst;

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(cv::Size(8, 8));
    clahe->apply(src, dst);

    return matToQImage(dst);
}

QImage ImageProcessor::apply(const QImage& image, ImageOperation operation)
{
    switch (operation) {
    case ImageOperation::EdgeDetection:
        return edgeEnhancement(image);

    case ImageOperation::EdgeLapDetection:
        return edgeEnhancementLap(image);

    case ImageOperation::Sharpen:
        return sharpen(image);

    case ImageOperation::ContrastEnhancement:
        return contrastEnhancement(image);

    case ImageOperation::FlipHorizontal:
        return flipHorizontal(image);

    case ImageOperation::FlipVertical:
        return flipVertical(image);

    case ImageOperation::Rotate90:
        return rotate90(image);

    case ImageOperation::Rotate180:
        return rotate180(image);

    case ImageOperation::Invert:
        return invert(image);

    case ImageOperation::DetailEnhancement:
        return detailEnhancement(image);

    case ImageOperation::Brightness:
        return brightness(image, 5);

    case ImageOperation::Amplifier:
        return amplifier(image, 1.05);

    case ImageOperation::NoiseReduction:
        return noiseReduction(image);

    case ImageOperation::DynamicRange:
        return dynamicRange(image);

    case ImageOperation::Crop:
        return image;
    }

    return image;
}