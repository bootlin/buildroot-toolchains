BASE_DIR := /home/test/workspace/src/kernel/output
MODULE_DIR := $(BASE_DIR)/build/corebld-$(COREBLD_VERSION)/test_module  

obj-m := dualcore_test.o
#include  $(BASE_DIR)/../board/AnalogDevices/blackfin/kernel-module.mk

all: module

module:
	echo $(LDFLAGS) 
	make ARCH=blackfin CC=bfin-uclinux-gcc LD=bfin-uclinux-ld CC="" LD="" CFLAGS="" CPPFLAGS="" LDFLAGS="" -C /home/test/workspace/src/kernel/output/build/linux-custom/ SUBDIRS=/home/test/workspace/src/kernel/output/build/corebld-HEAD/test_module modules

clean:
        rm -f *.ko *.o *.mod*

