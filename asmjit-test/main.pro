QT += quick gui widgets

CONFIG += c++17
CONFIG += console
CONFIG += force_debug_info

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += _WINSOCKAPI_

LIBS += -L$$[QT_INSTALL_PREFIX]/lib

gcc:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-function
msvc:QMAKE_LFLAGS += /LTCG:INCREMENTAL

include($$PWD/../asmjit.pri)
include($$(HOME)/include/include.pri)

SOURCES += \
        gc_test.cpp \
        main.cpp

HEADERS += \
    app-data.h \
    gc_test.h \
    test_fib_dbl.h \
    test_fib_i32.h \
    test_fib_i64.h \
    test_fib_u32.h \
    test_fib_u64.h \
    test_initval.h \
    test_pointer.h \
    test_string.h \
    ult_jit.h

QMAKE_LIBDIR += $$PWD/tmp

#DEFINES += ASMJIT_STATIC
DEFINES += DEBUG_LINE

#LIBS += -lasmjit -lgc -lgccpp
#LIBS += -lasmjit
LIBS += -lgc -lgccpp

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

RESOURCES += \
    main.qrc
