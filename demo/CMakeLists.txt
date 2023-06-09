cmake_minimum_required(VERSION 3.5)
project(RibbonDemo VERSION ${VERSION_SHORT})

set(QX_RIBBON_DEMO_NAME RibbonDemo)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(PROJECT_SOURCES
    mainwindow.h
    mainwindow.cpp
    aboutdialog.h
    aboutdialog.cpp
    main.cpp
    demo.qrc
)

add_executable(${QX_RIBBON_DEMO_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${QX_RIBBON_DEMO_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

find_library(QxRibbon NAMES QxRibbon)
target_link_libraries(${QX_RIBBON_DEMO_NAME} PRIVATE QxRibbon)

set_target_properties(${QX_RIBBON_DEMO_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_EXTENSIONS OFF
    EXPORT_NAME ${QX_RIBBON_DEMO_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${QX_RIBBON_BIN_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${QX_RIBBON_BIN_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${QX_RIBBON_BIN_DIR}/bin"
)
