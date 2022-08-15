QT += core gui widgets network

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++17
CONFIG += force_debug_info

gcc:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-function

DEFINES += CPPHTTPLIB_OPENSSL_SUPPORT
DEFINES += HTTP_API_USE_QT

INCLUDEPATH += $$PWD
INCLUDEPATH += $$(HOME)/include

msvc:INCLUDEPATH += $$(HOME)/conan/binaries/include
msvc:LIBS += -L$$(HOME)/conan/binaries/lib libcrypto.lib libssl.lib
gcc:LIBS += -lssl -lcrypto

DESTDIR += $$PWD

HEADERS += \
    $$PWD/JSValueToText.h \
    $$PWD/jchecksum.h \
    $$PWD/jlistwidget.h \
    $$PWD/jnetwork.h \
    $$PWD/junctionmanager.h \
    $$PWD/pcloudapi.h \
    $$PWD/qdebug.h \
    $$PWD/utf8LogHandler.h \
    $$PWD/variantserializer.h \
    $$PWD/varianttojson.h \
    $$PWD/youtubeapi.h \
    MemoryModule.h \
    archive_api.h \
    http_api.h \
    jarchiver.h

SOURCES += \
    $$PWD/jchecksum.cpp \
    $$PWD/jlistwidget.cpp \
    $$PWD/jnetwork.cpp \
    $$PWD/junctionmanager.cpp \
    $$PWD/pcloudapi.cpp \
    $$PWD/variantserializer.cpp \
    $$PWD/youtubeapi.cpp \
    MemoryModule.c \
    archive_api.cpp \
    http_api.cpp \
    jarchiver.cpp

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
