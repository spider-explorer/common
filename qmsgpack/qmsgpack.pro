#QT += core gui widgets network
QT += core

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++17
CONFIG += force_debug_info

gcc:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-function

DEFINES += MSGPACK_STATIC

INCLUDEPATH += $$PWD/src

DESTDIR += $$PWD

HEADERS += \
    src/endianhelper.h \
    src/msgpack.h \
    src/msgpack_export.h \
    src/msgpackcommon.h \
    src/msgpackcommon.h.in \
    src/msgpackstream.h \
    src/private/pack_p.h \
    src/private/qt_types_p.h \
    src/private/unpack_p.h \
    src/stream/geometry.h \
    src/stream/location.h \
    src/stream/time.h

SOURCES += \
    src/msgpack.cpp \
    src/msgpackcommon.cpp \
    src/msgpackstream.cpp \
    src/private/pack_p.cpp \
    src/private/qt_types_p.cpp \
    src/private/unpack_p.cpp \
    src/stream/geometry.cpp \
    src/stream/location.cpp \
    src/stream/time.cpp

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
