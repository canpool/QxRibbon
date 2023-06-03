QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

OUT_ROOT = $${OUT_PWD}/..
TARGET = RibbonDemo
TEMPLATE = app
DESTDIR = $${OUT_ROOT}/bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    aboutdialog.h \
    mainwindow.h

include($$PWD/../common.pri)
LIBS += -L$${OUT_ROOT}/bin -l$$qtLibraryNameVersion(QxRibbon, 0)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    demo.qrc

RC_FILE = demo.rc
