lessThan(QT_MAJOR_VERSION, 5) {
    error("don't support qt $${QT_VERSION}")
}

OUT_ROOT = $${OUT_PWD}/..
TARGET = $$qtLibraryTarget(QxRibbon)
TEMPLATE = lib
DESTDIR = $${OUT_ROOT}/bin

include($$PWD/QxRibbon.pri)
