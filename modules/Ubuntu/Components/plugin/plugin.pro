TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += declarative core gui
CONFIG += qt plugin no_keywords

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h

SOURCES += plugin.cpp

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/plugin/lib
    INSTALLS += target
}
