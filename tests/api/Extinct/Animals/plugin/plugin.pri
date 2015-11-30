QT *= core-private qml qml-private quick quick-private gui-private testlib

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT *= v8-private
}

CONFIG  += no_keywords
DEFINES += UBUNTU_QML_IMPORT_PATH='\\"$${ROOT_BUILD_DIR}/qml\\"'
DEFINES += QT_USE_QSTRINGBUILDER

TARGET = $$qtLibraryTarget($$TARGET)
uri = Extinct.Animals

HEADERS += \
    $$PWD/plugin.h \
    $$PWD/smilodon.h \

SOURCES += \
    $$PWD/plugin.cpp \
    $$PWD/smilodon.cpp \
