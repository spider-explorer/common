QT += gui widgets

CONFIG += c++17 console

MSYSTEM = $$(MSYSTEM)
TARGET_BIT = $$replace(MSYSTEM, "MINGW", "")
TARGET = $${TARGET}-x$${TARGET_BIT}

SOURCES += \
        asmjit_test_compiler.cpp asmjit_test_compiler_x86.cpp

HEADERS += \
    asmjit_test_compiler.h

QMAKE_LIBDIR += $$PWD/tmp

DEFINES += ASMJIT_STATIC

LIBS += -lasmjit

DESTDIR = $$PWD/tmp.$$(MSYSTEM_CARCH)

#message($$QMAKE_QMAKE)
contains(QMAKE_QMAKE, .*\/qt5-static\/.*) {
    message( "[STATIC BUILD]" )
    DEFINES += QT_STATIC_BUILD
} else {
    message( "[SHARED BUILD]" )
}
