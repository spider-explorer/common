# https://github.com/mutouyun/cpp-ipc

INCLUDEPATH += $$PWD

#gcc:QMAKE_LFLAGS += -Wl,--allow-multiple-definition

HEADERS += \
    $$PWD/libipc/buffer.h \
    $$PWD/libipc/circ/elem_array.h \
    $$PWD/libipc/circ/elem_def.h \
    $$PWD/libipc/condition.h \
    $$PWD/libipc/def.h \
    $$PWD/libipc/export.h \
    $$PWD/libipc/ipc.h \
    $$PWD/libipc/memory/alloc.h \
    $$PWD/libipc/memory/allocator_wrapper.h \
    $$PWD/libipc/memory/resource.h \
    $$PWD/libipc/memory/wrapper.h \
    $$PWD/libipc/mutex.h \
    $$PWD/libipc/platform/detail.h \
    $$PWD/libipc/platform/win/condition.h \
    $$PWD/libipc/platform/win/get_sa.h \
    $$PWD/libipc/platform/win/mutex.h \
    $$PWD/libipc/platform/win/semaphore.h \
    $$PWD/libipc/platform/win/to_tchar.h \
    $$PWD/libipc/policy.h \
    $$PWD/libipc/pool_alloc.h \
    $$PWD/libipc/prod_cons.h \
    $$PWD/libipc/queue.h \
    $$PWD/libipc/rw_lock.h \
    $$PWD/libipc/semaphore.h \
    $$PWD/libipc/shm.h \
    $$PWD/libipc/utility/concept.h \
    $$PWD/libipc/utility/id_pool.h \
    $$PWD/libipc/utility/log.h \
    $$PWD/libipc/utility/pimpl.h \
    $$PWD/libipc/utility/scope_guard.h \
    $$PWD/libipc/utility/utility.h \
    $$PWD/libipc/waiter.h

SOURCES += \
    $$PWD/libipc/buffer.cpp \
    $$PWD/libipc/ipc.cpp \
    $$PWD/libipc/platform/platform.cpp \
    $$PWD/libipc/pool_alloc.cpp \
    $$PWD/libipc/shm.cpp \
    $$PWD/libipc/sync/condition.cpp \
    $$PWD/libipc/sync/mutex.cpp \
    $$PWD/libipc/sync/semaphore.cpp
