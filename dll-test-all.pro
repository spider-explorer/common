TEMPLATE = subdirs
SUBDIRS += exe
SUBDIRS += dll
exe.depends += dll
SUBDIRS += $$(REPO)/common/common
dll.depends += $$(REPO)/common/common
exe.depends += $$(REPO)/common/common
