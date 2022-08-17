INCLUDEPATH += $$PWD/qcbor/inc

HEADERS += \
    $$PWD/qcbor/inc/UsefulBuf.h \
    $$PWD/qcbor/inc/qcbor/UsefulBuf.h \
    $$PWD/qcbor/inc/qcbor/qcbor.h \
    $$PWD/qcbor/inc/qcbor/qcbor_common.h \
    $$PWD/qcbor/inc/qcbor/qcbor_decode.h \
    $$PWD/qcbor/inc/qcbor/qcbor_encode.h \
    $$PWD/qcbor/inc/qcbor/qcbor_private.h \
    $$PWD/qcbor/inc/qcbor/qcbor_spiffy_decode.h \
    $$PWD/qcbor/src/ieee754.h

SOURCES += \
    $$PWD/qcbor/src/UsefulBuf.c \
    $$PWD/qcbor/src/ieee754.c \
    $$PWD/qcbor/src/qcbor_decode.c \
    $$PWD/qcbor/src/qcbor_encode.c \
    $$PWD/qcbor/src/qcbor_err_to_str.c

