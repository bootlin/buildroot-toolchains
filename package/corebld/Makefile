obj-m := dualcore_test.o
#include  $(BASE_DIR)/../board/AnalogDevices/blackfin/kernel-module.mk
COREBLD_BUILD_DIR=$(shell pwd)

all: module

module:
	make ARCH=blackfin LDFLAGS="" -C $(BASE_DIR)/build/linux-custom/  SUBDIRS=$(COREBLD_BUILD_DIR) modules
clean:
	rm -f *.ko *.o *.mod*
