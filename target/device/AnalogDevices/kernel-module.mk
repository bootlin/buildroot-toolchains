#ifeq ($(origin CC),default)
CC := bfin-uclinux-gcc
LD := bfin-uclinux-ld
#endif
MAKEARCH_KERNEL ?= make ARCH=blackfin CC=bfin-uclinux-gcc LD=bfin-uclinux-ld 
LINUXDIR ?= build/linux-custom/

ROOT_BASE_DIR=$(BASE_DIR)
# avoid infinite recursion
ifneq ($(LINUXDIR),)
MAKE_KERNEL = CC="" LD="" CFLAGS="" CPPFLAGS="" LDFLAGS="" \
	$(MAKEARCH_KERNEL) -C $(ROOT_BASE_DIR)/$(LINUXDIR) SUBDIRS=$$MODULE_DIR
else
MAKE_KERNEL = echo
endif

EXTRA_CFLAGS += -Wall

all: module

module:
	 $(MAKE_KERNEL) modules

clean:
	rm -f *.o *.ko *.gdb
	$(MAKE_KERNEL) clean

ROMFS_MODULES ?= $(obj-m:.o=.ko)
romfs::
	for m in $(ROMFS_MODULES) ; do $(ROMFSINST) -d -M $$m misc/$$m || exit $$? ; done

.PHONY: all clean module romfs
