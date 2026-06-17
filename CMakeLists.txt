cmake_minimum_required(VERSION 3.19)
project(MiniMedicalImageViewer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(DCMTK CONFIG REQUIRED)

qt_standard_project_setup()

qt_add_executable(MiniMedicalImageViewer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    imageviewer.h
    imageviewer.cpp

)

target_link_libraries(MiniMedicalImageViewer
    PRIVATE
        Qt::Core
        Qt::Widgets
        DCMTK::DCMTK
)

install(TARGETS MiniMedicalImageViewer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MiniMedicalImageViewer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
