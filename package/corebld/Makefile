obj-m := dualcore_test.o
#include  $(BASE_DIR)/../board/AnalogDevices/blackfin/kernel-module.mk

all: module

module:
	make ARCH=blackfin LDFLAGS="" -C $(BASE_DIR)/build/linux-custom/  SUBDIRS=$(BASE_DIR)/build/corebld-HEAD/test_module modules
clean:
	rm -f *.ko *.o *.mod*
