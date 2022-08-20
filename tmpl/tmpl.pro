TEMPLATE = subdirs
SUBDIRS += exe
SUBDIRS += lib
exe.depends += lib
SUBDIRS += $$(REPO)/common/common
lib.depends += $$(REPO)/common/common
exe.depends += $$(REPO)/common/common
