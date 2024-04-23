cmake_minimum_required(VERSION 3.5)

project(QxRibbon LANGUAGES CXX VERSION ${QX_RIBBON_VERSION})
set(QX_RIBBON_LIB_NAME QxRibbon${PROJECT_VERSION_MAJOR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(QX_RIBBON_PUBLIC_HEADERS
    QxRibbonActionsManager.h
    QxRibbonBar.h
    QxRibbonButton.h
    QxRibbonButtonGroup.h
    QxRibbonColorButton.h
    QxRibbonContainers.h
    QxRibbonControls.h
    QxRibbonCustomizeDialog.h
    QxRibbonFactory.h
    QxRibbonGroup.h
    QxRibbonManager.h
    QxRibbonPage.h
    QxRibbonPageContext.h
    QxRibbonStyleOption.h
    QxRibbonTheme.h
    QxRibbonUtils.h
    QxRibbonWindow.h
    QxRibbonApplicationButton.h
    QxRibbonTabBar.h
    QxRibbonGallery.h
    QxRibbonGalleryGroup.h
    QxRibbonDrawHelper.h
    QxRibbonQuickAccessBar.h
)

set(QX_RIBBON_PRIVATE_HEADERS
    FramelessHelper.h
    QxRibbonBarPrivate.h
    QxRibbonButtonPrivate.h
    QxRibbonColorButtonPrivate.h
    QxRibbonCustomizeData.h
    QxRibbonCustomizeWidget.h
    QxRibbonGalleryGroupPrivate.h
    QxRibbonGroupLayout.h
    QxRibbonGroupPrivate.h
    QxRibbonPagePrivate.h
    QxRibbonQuickAccessBarPrivate.h
    QxWindowButtonGroup.h
    QxWindowButtonGroupPrivate.h
)

set(QX_RIBBON_SOURCES
    FramelessHelper.cpp
    QxRibbonActionsManager.cpp
    QxRibbonBar.cpp
    QxRibbonButton.cpp
    QxRibbonButtonGroup.cpp
    QxRibbonColorButton.cpp
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
    QxRibbonTheme.cpp
    QxRibbonUtils.cpp
    QxRibbonWindow.cpp
    QxWindowButtonGroup.cpp
    QxRibbonApplicationButton.cpp
    QxRibbonTabBar.cpp
    QxRibbonGallery.cpp
    QxRibbonGalleryGroup.cpp
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

if (WIN32)
    if (Qt5_VERSION VERSION_LESS 6.0.0)
        target_compile_definitions(${QX_RIBBON_LIB_NAME} PRIVATE FRAMELESS_USE_NATIVE)
    endif()
endif()

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
   VERSION ${QX_RIBBON_VERSION}
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
