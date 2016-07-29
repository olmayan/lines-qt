QT += widgets svg
CONFIG += c++11
TEMPLATE = app
TARGET = lines-qt

SOURCES += main.cpp \
    mainwindow.cpp \
    field.cpp \
    svgrendereritem.cpp \
    resourceloader.cpp \
    graphicsview.cpp

RESOURCES += \
    lines_qt.qrc

HEADERS += \
    mainwindow.h \
    field.h \
    svgrendereritem.h \
    resourceloader.h \
    graphicsview.h
