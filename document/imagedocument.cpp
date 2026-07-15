#include "document/imagedocument.h"

#include "dicom/dicomloader.h"
#include "processing/imageprocessor.h"

#include <algorithm>

bool ImageDocument::load(
    const QString& filePath,
    QString& errorMessage
    )
{
    DicomImageData loadedData;

    if (!DicomLoader::load(filePath, loadedData, errorMessage))
        return false;

    originalDicom = loadedData;
    currentDicom = loadedData;

    rebuildDisplayImage();
    return true;
}

void ImageDocument::reset()
{
    if (!originalDicom.isValid())
        return;

    currentDicom = originalDicom;
    rebuildDisplayImage();
}

bool ImageDocument::isLoaded() const
{
    return currentDicom.isValid();
}

const DicomImageData& ImageDocument::dicom() const
{
    return currentDicom;
}

const QImage& ImageDocument::displayImage() const
{
    return currentDisplayImage;
}

void ImageDocument::adjustWindowLevel(
    int deltaX,
    int deltaY
    )
{
    if (!currentDicom.isValid())
        return;

    constexpr double widthStep = 1.0;
    constexpr double levelStep = 1.0;

    currentDicom.windowWidth += deltaX * widthStep;
    currentDicom.windowCenter -= deltaY * levelStep;

    currentDicom.windowWidth =
        std::max(1.0, currentDicom.windowWidth);

    currentDicom.windowCenter = std::clamp(
        currentDicom.windowCenter,
        currentDicom.minValue,
        currentDicom.maxValue
        );

    rebuildDisplayImage();
}

void ImageDocument::applyOperation(ImageOperation operation)
{
    if (currentDisplayImage.isNull())
        return;

    if (operation == ImageOperation::Crop)
        return;

    currentDisplayImage =
        ImageProcessor::apply(currentDisplayImage, operation);
}

bool ImageDocument::crop(const QRect& rect)
{
    if (currentDisplayImage.isNull())
        return false;

    const QRect validRect =
        rect.intersected(currentDisplayImage.rect());

    if (validRect.isEmpty())
        return false;

    currentDisplayImage =
        currentDisplayImage.copy(validRect);

    return true;
}

void ImageDocument::rebuildDisplayImage()
{
    if (!currentDicom.isValid()) {
        currentDisplayImage = QImage();
        return;
    }

    currentDisplayImage = ImageProcessor::applyWindowLevel(
        currentDicom,
        currentDicom.windowWidth,
        currentDicom.windowCenter
        );
}