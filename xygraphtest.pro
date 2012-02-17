QT       += gui

TARGET = xygraph
TEMPLATE = app

DEFINES += XYGRAPH_LIBRARY

SOURCES += xyscene.cpp \
    xygraph.cpp \
    main.cpp \
    xyspline.cpp

HEADERS += realzoom.hpp \
    xygraph_global.hpp \
    xygraph.hpp \
    xyscene.hpp \
    xyspline.hpp

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEF46DA07
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = lockin2.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
        headers.files = realzoom.hpp xygraph.hpp xyscene.hpp xygraph_global.hpp
        headers.path = /usr/local/include/xygraph
    }
    INSTALLS += target headers
}

LIBS += -lgsl -lgslcblas

## lancer la commande 'sudo ldconfig' pour résoudre le problème d'execution

