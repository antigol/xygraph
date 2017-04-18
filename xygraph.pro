QT       += gui widgets

TARGET = xygraph
TEMPLATE = app

include(xygraph.pri)

SOURCES += main.cc

# disable all the deprecated APIs in Qt <= 5.8
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050800
