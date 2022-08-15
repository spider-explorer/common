QT += core gui widgets
equals(QT_MAJOR_VERSION, 6):QT += core5compat

CONFIG += c++17
CONFIG += console
CONFIG += force_debug_info

#TEMPLATE = lib
#CONFIG += staticlib
#CONFIG += dll

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += _WINSOCKAPI_
DEFINES += DEBUG_LINE

gcc:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-function -Wno-cast-function-type
msvc:QMAKE_CXXFLAGS += /bigobj

HEADERS += utf8LogHandler.h

SOURCES += test.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$(HOME)/include
#QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_DEBUG
LIBS += -L$$[QT_INSTALL_PREFIX]/lib

DESTDIR = $$PWD

TARGET = $${TARGET}-$${QMAKE_HOST.arch}
#message($$QMAKE_QMAKE)
contains(QMAKE_QMAKE, .*static.*) {
    message( "[STATIC BUILD]" )
    DEFINES += QT_STATIC_BUILD
    TARGET = $${TARGET}-static
} else {
    message( "[SHARED BUILD]" )
}

#gcc:QMAKE_POST_LINK=strip $$DESTDIR/$(TARGET)

include($$(REPO)/common/include/include.pri)
include($$(REPO)/common/common/common.pri)
#include($$(REPO)/common//common/boost/boost.pri)

include($$(REPO)/common/qmsgpack/qmsgpack.pri)

LIBS += -lffi
