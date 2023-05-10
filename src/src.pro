QT += widgets

OUT_ROOT = $${OUT_PWD}/..
TARGET = $$qtLibraryTarget(QxRibbon)
TEMPLATE = lib
DESTDIR = $${OUT_ROOT}/bin

DEFINES += QX_RIBBON_LIBRARY

include($$PWD/QxRibbon.pri)
