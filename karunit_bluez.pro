TEMPLATE        = lib
CONFIG         += plugin c++17
DEFINES        += QT_DEPRECATED_WARNINGS
QT             += BluezQt core widgets dbus bluetooth
TARGET          = karunit_bluez_plugin
DESTDIR         = $$PWD/../karunit/app/plugins

LIBS += -L$$PWD/../karunit/plugininterface/ -lplugininterface
INCLUDEPATH += $$PWD/../karunit/plugininterface

LIBS += -L$$PWD/../karunit/common/ -lcommon
INCLUDEPATH += $$PWD/../karunit/common

LIBS += -L$$PWD/../karunit/third-party/xblog/lib -lxblog
INCLUDEPATH += $$PWD/../karunit/third-party/xblog/include

SUBDIRS += \
    src/

include(src/src.pri)
