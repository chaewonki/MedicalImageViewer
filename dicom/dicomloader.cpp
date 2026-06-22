#include "dicomloader.h"

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <limits>
#include <cmath>

bool DicomLoader::load(const QString& filePath, DicomImageData& outData, QString& errorMessage)
{
    DcmFileFormat fileFormat;

    OFCondition status = fileFormat.loadFile(filePath.toStdString().c_str());
    if (!status.good()) {
        errorMessage = "Failed to load DICOM file";
        return false;
    }

    DcmDataset* dataset = fileFormat.getDataset();

    Uint16 rows = 0;
    Uint16 cols = 0;

    dataset->findAndGetUint16(DCM_Rows, rows);
    dataset->findAndGetUint16(DCM_Columns, cols);

    if (rows == 0 || cols == 0) {
        errorMessage = "Invalid image size";
        return false;
    }

    // X-ray defalut
    double slope = 1.0;
    double intercept = 0.0;
    dataset->findAndGetFloat64(DCM_RescaleSlope, slope);
    dataset->findAndGetFloat64(DCM_RescaleIntercept, intercept);
    OFString photometric;
    dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometric);
    outData.inverted = photometric == "MONOCHROME1";

    Uint16 bitsStored = 16;
    if (!dataset->findAndGetUint16(DCM_BitsStored, bitsStored).good() ||
        bitsStored == 0 || bitsStored > 16) {
        bitsStored = 16;
    }
    outData.bitsStored = static_cast<int>(bitsStored);

    const Uint16* pixelData = nullptr;
    unsigned long pixelCount = 0;

    status = dataset->findAndGetUint16Array(DCM_PixelData, pixelData, &pixelCount);

    if (!status.good() || !pixelData) {
        errorMessage = "Failed to get raw pixel data";
        return false;
    }

    const int width = cols;
    const int height = rows;
    const int expectedCount = width * height;

    if (pixelCount < static_cast<unsigned long>(expectedCount)) {
        errorMessage = "Pixel data size mismatch";
        return false;
    }

    outData.width = width;
    outData.height = height;
    outData.pixels.resize(expectedCount);

    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (int i = 0; i < expectedCount; ++i) {
        double value = static_cast<double>(pixelData[i]);
        value = value * slope + intercept;

        outData.pixels[i] = value;

        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
    }

    outData.minValue = minValue;
    outData.maxValue = maxValue;

    double windowWidth = 0.0;
    double windowCenter = 0.0;

    const bool hasWindowWidth =
        dataset->findAndGetFloat64(DCM_WindowWidth, windowWidth, 0).good();

    const bool hasWindowCenter =
        dataset->findAndGetFloat64(DCM_WindowCenter, windowCenter, 0).good();

    if (hasWindowWidth && hasWindowCenter &&
        std::isfinite(windowWidth) && std::isfinite(windowCenter) &&
        windowWidth > 0.0) {
        outData.windowWidth = windowWidth;
        outData.windowCenter = windowCenter;
    } else {
        outData.windowWidth = maxValue - minValue;
        outData.windowCenter = (maxValue + minValue) / 2.0;

        if (outData.windowWidth <= 0.0)
            outData.windowWidth = 1.0;
    }

    return true;
}