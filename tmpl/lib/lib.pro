QT += core

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++17
CONFIG += force_debug_info

gcc:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-function

INCLUDEPATH += $$PWD

#gcc:LIBS += -lssl -lcrypto

DESTDIR += $$PWD

TARGET = $${TARGET}$${QT_MAJOR_VERSION}-$${QMAKE_HOST.arch}
#message($$QMAKE_QMAKE)
contains(QMAKE_QMAKE, .*static.*) {
    message( "[STATIC BUILD]" )
    DEFINES += QT_STATIC_BUILD
    TARGET = $${TARGET}-static
} else {
    message( "[SHARED BUILD]" )
}
message($$TARGET)
MOC_DIR = build-$${TARGET}
OBJECTS_DIR = build-$${TARGET}
RCC_DIR = build-$${TARGET}
UI_DIR = build-$${TARGET}

include($$(REPO)/common/boost/boost.pri)
include($$(REPO)/common/common/common.pri)

HEADERS += \
    mylib.h

SOURCES += \
    mylib.cpp
