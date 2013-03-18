uri = Ubuntu.Tests
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# Components
QML_FILES = $$system(ls *.qml)
JS_FILES = $$system(ls *.js)

# qmldir
QMLDIR_FILE = qmldir

# make found deployables visible in Qt Creator
OTHER_FILES += $$QML_FILES $$JS_FILES $$QMLDIR_FILE

# define deployment for found deployables
qmldir_file.path = $$installPath
qmldir_file.files = $$QMLDIR_FILE
qml_files.path = $$installPath
qml_files.files = $$QML_FILES
js_files.path = $$installPath
js_files.files = $$JS_FILES

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
plugins_qmltypes.extra = $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.Tests 0.1 ../../ > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes

INSTALLS += qmldir_file qml_files js_files plugins_qmltypes
