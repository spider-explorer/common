TEMPLATE = subdirs
SUBDIRS += test
SUBDIRS += $$(REPO)/common/common
test.depends += $$(REPO)/common/common
SUBDIRS += ./qmsgpack
test.depends += ./qmsgpack
