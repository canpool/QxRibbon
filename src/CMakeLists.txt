cmake_minimum_required(VERSION 3.5)

project(QxRibbon LANGUAGES CXX VERSION ${QX_VERSION_RIBBON})
set(QX_RIBBON_LIB_NAME QxRibbon)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(QX_RIBBON_PUBLIC_HEADERS
    QxGlobal.h
    QxRibbonActionsManager.h
    QxRibbonBar.h
    QxRibbonButton.h
    QxRibbonButtonGroup.h
    QxRibbonContainers.h
    QxRibbonControls.h
    QxRibbonCustomizeData.h
    QxRibbonCustomizeDialog.h
    QxRibbonCustomizeWidget.h
    QxRibbonFactory.h
    QxRibbonGroup.h
    QxRibbonManager.h
    QxRibbonPage.h
    QxRibbonPageContext.h
    QxRibbonStyleOption.h
    QxRibbonWindow.h
    QxRibbonApplicationButton.h
    QxRibbonTabBar.h
    QxRibbonGallery.h
    QxRibbonGalleryGroup.h
    QxRibbonGalleryItem.h
    QxRibbonDrawHelper.h
    QxRibbonQuickAccessBar.h
)

set(QX_RIBBON_PRIVATE_HEADERS
    FramelessHelper.h
    QxRibbonBarPrivate.h
    QxRibbonGroupLayout.h
    QxRibbonGroupPrivate.h
    QxRibbonPagePrivate.h
    QxWindowButtonGroup.h
)

set(QX_RIBBON_SOURCES
    FramelessHelper.cpp
    QxRibbonActionsManager.cpp
    QxRibbonBar.cpp
    QxRibbonButton.cpp
    QxRibbonButtonGroup.cpp
    QxRibbonContainers.cpp
    QxRibbonControls.cpp
    QxRibbonCustomizeData.cpp
    QxRibbonCustomizeDialog.cpp
    QxRibbonCustomizeWidget.cpp
    QxRibbonFactory.cpp
    QxRibbonGroup.cpp
    QxRibbonGroupLayout.cpp
    QxRibbonManager.cpp
    QxRibbonPage.cpp
    QxRibbonPageContext.cpp
    QxRibbonStyleOption.cpp
    QxRibbonWindow.cpp
    QxWindowButtonGroup.cpp
    QxRibbonApplicationButton.cpp
    QxRibbonTabBar.cpp
    QxRibbonGallery.cpp
    QxRibbonGalleryGroup.cpp
    QxRibbonGalleryItem.cpp
    QxRibbonDrawHelper.cpp
    QxRibbonQuickAccessBar.cpp
)

set(QX_RIBBON_RESOURCES
    res.qrc
)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_library(${QX_RIBBON_LIB_NAME} SHARED
    ${QX_RIBBON_PUBLIC_HEADERS}
    ${QX_RIBBON_PRIVATE_HEADERS}
    ${QX_RIBBON_SOURCES}
    ${QX_RIBBON_RESOURCES}
)

target_compile_definitions(${QX_RIBBON_LIB_NAME} PRIVATE QX_RIBBON_LIBRARY)

target_include_directories(${QX_RIBBON_LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${QX_RIBBON_LIB_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(${QX_RIBBON_LIB_NAME} PROPERTIES
   AUTOMOC ON
   AUTOUIC ON
   AUTORCC ON
   CXX_EXTENSIONS OFF
   VERSION ${QX_VERSION_RIBBON}
   EXPORT_NAME ${QX_RIBBON_LIB_NAME}
   ARCHIVE_OUTPUT_DIRECTORY "${QX_RIBBON_BIN_DIR}/lib"
   LIBRARY_OUTPUT_DIRECTORY "${QX_RIBBON_BIN_DIR}/lib"
   RUNTIME_OUTPUT_DIRECTORY "${QX_RIBBON_BIN_DIR}/bin"
)

install(FILES ${QX_RIBBON_PUBLIC_HEADERS}
    DESTINATION include/${QX_RIBBON_LIB_NAME}
    COMPONENT headers
)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
    DESTINATION license/${QX_RIBBON_LIB_NAME}
    COMPONENT license
)

install(TARGETS ${QX_RIBBON_LIB_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${QX_RIBBON_LIB_NAME}
)

target_include_directories(${QX_RIBBON_LIB_NAME} PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)