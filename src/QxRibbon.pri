VERSION = 0.5.4

QT += widgets

DEFINES += QX_RIBBON_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    $$PWD/FramelessHelper.cpp \
    $$PWD/QxRibbonActionsManager.cpp \
    $$PWD/QxRibbonBar.cpp \
    $$PWD/QxRibbonButton.cpp \
    $$PWD/QxRibbonButtonGroup.cpp \
    $$PWD/QxRibbonContainers.cpp \
    $$PWD/QxRibbonControls.cpp \
    $$PWD/QxRibbonCustomizeData.cpp \
    $$PWD/QxRibbonCustomizeDialog.cpp \
    $$PWD/QxRibbonCustomizeWidget.cpp \
    $$PWD/QxRibbonFactory.cpp \
    $$PWD/QxRibbonGroup.cpp \
    $$PWD/QxRibbonGroupLayout.cpp \
    $$PWD/QxRibbonManager.cpp \
    $$PWD/QxRibbonPage.cpp \
    $$PWD/QxRibbonPageContext.cpp \
    $$PWD/QxRibbonStyleOption.cpp \
    $$PWD/QxRibbonWindow.cpp \
    $$PWD/QxWindowButtonGroup.cpp \
    $$PWD/QxRibbonApplicationButton.cpp \
    $$PWD/QxRibbonTabBar.cpp \
    $$PWD/QxRibbonGallery.cpp \
    $$PWD/QxRibbonGalleryGroup.cpp \
    $$PWD/QxRibbonGalleryItem.cpp \
    $$PWD/QxRibbonDrawHelper.cpp \
    $$PWD/QxRibbonQuickAccessBar.cpp

PUBLIC_HEADERS = \
    $$PWD/QxGlobal.h \
    $$PWD/QxRibbonActionsManager.h \
    $$PWD/QxRibbonBar.h \
    $$PWD/QxRibbonButton.h \
    $$PWD/QxRibbonButtonGroup.h \
    $$PWD/QxRibbonContainers.h \
    $$PWD/QxRibbonControls.h \
    $$PWD/QxRibbonCustomizeDialog.h \
    $$PWD/QxRibbonFactory.h \
    $$PWD/QxRibbonGlobal.h \
    $$PWD/QxRibbonGroup.h \
    $$PWD/QxRibbonManager.h \
    $$PWD/QxRibbonPage.h \
    $$PWD/QxRibbonPageContext.h \
    $$PWD/QxRibbonStyleOption.h \
    $$PWD/QxRibbonWindow.h \
    $$PWD/QxRibbonApplicationButton.h \
    $$PWD/QxRibbonTabBar.h \
    $$PWD/QxRibbonGallery.h \
    $$PWD/QxRibbonGalleryGroup.h \
    $$PWD/QxRibbonGalleryItem.h \
    $$PWD/QxRibbonDrawHelper.h \
    $$PWD/QxRibbonQuickAccessBar.h

PRIVATE_HEADERS = \
    $$PWD/FramelessHelper.h \
    $$PWD/QxRibbonBarPrivate.h \
    $$PWD/QxRibbonCustomizeData.h \
    $$PWD/QxRibbonCustomizeWidget.h \
    $$PWD/QxRibbonGroupLayout.h \
    $$PWD/QxRibbonGroupPrivate.h \
    $$PWD/QxRibbonPagePrivate.h \
    $$PWD/QxWindowButtonGroup.h

HEADERS  += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

RESOURCES += \
    $$PWD/res.qrc

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

win32 {
    LIBS += -lUser32
}
