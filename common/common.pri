QT += core gui widgets network

INCLUDEPATH += $$PWD

msvc:INCLUDEPATH += $$(HOME)/conan/binaries/include
msvc:LIBS += -L$$(HOME)/conan/binaries/lib libcrypto.lib libssl.lib archive.lib

DEFINES += HTTP_API_USE_QT

LIBNAME = common
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
#msvc:LIBS += $$PWD/common.lib
#gcc:LIBS += $$PWD/libcommon.a
LIBS += -L$$PWD -l$$LIBNAME

gcc {
    LIBS += -larchive -lz -lbz2 -llzma -liconv -lbcrypt -lexpat -lb2 -llz4
    LIBS += -lssl -lcrypto
    LIBS += -ladvapi32 -lole32 -loleaut32 -luser32 -luuid
}
msvc {
    #LIBS += archive_static.lib
    #LIBS += archive.lib
    LIBS += -ladvapi32 # CreateJunction()
}
