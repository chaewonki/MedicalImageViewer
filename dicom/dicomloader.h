#ifndef DICOMLOADER_H
#define DICOMLOADER_H

#include "dicom/dicomimagedata.h"
#include <QString>

class DicomLoader{
public:
    static bool load(const QString& filePath, DicomImageData& outData, QString& errorMessage);
};

#endif // DICOMLOADER_H
