QT += core

INCLUDEPATH += $$PWD

LIBNAME = lib
LIBNAME = $${LIBNAME}$${QT_MAJOR_VERSION}-$${QMAKE_HOST.arch}
#message($$QMAKE_QMAKE)
contains(QMAKE_QMAKE, .*static.*) {
    message( "[STATIC BUILD]" )
    DEFINES += QT_STATIC_BUILD
    LIBNAME = $${LIBNAME}-static
} else {
    message( "[SHARED BUILD]" )
}
message($$LIBNAME)
LIBS += -L$$PWD -l$$LIBNAME
