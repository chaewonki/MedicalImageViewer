cmake_minimum_required(VERSION 3.19)
project(MedicalImageViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts)
find_package(DCMTK CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

qt_standard_project_setup()

qt_add_executable(MedicalImageViewer
    WIN32 MACOSX_BUNDLE
    main.cpp

    ui/mainwindow.h
    ui/mainwindow.cpp
    ui/imageviewer.h
    ui/imageviewer.cpp

    dicom/dicomimagedata.h
    dicom/dicomloader.h
    dicom/dicomloader.cpp

    processing/imageprocessor.h
    processing/imageprocessor.cpp

    ui/controlpanel.h
    ui/controlpanel.cpp

    processing/imageoperation.h

    analysis/roistatistics.h
    analysis/roianalyzer.h
    analysis/roianalyzer.cpp
    ui/roihistogramdialog.h
    ui/roihistogramdialog.cpp
    document/imagedocument.h
    document/imagedocument.cpp

)

target_include_directories(MedicalImageViewer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MedicalImageViewer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Charts
        DCMTK::DCMTK
        ${OpenCV_LIBS}
)

install(TARGETS MedicalImageViewer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MedicalImageViewer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
