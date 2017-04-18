QT       += gui widgets

TARGET = xygraph
TEMPLATE = app

SOURCES += xyscene.cc \
    xyview.cc \
    main.cc \
    xygraph.cc

HEADERS += \
    xyscene.hh \
    xyview.hh \
    xygraph.hh

# disable all the deprecated APIs in Qt <= 5.8
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050800
