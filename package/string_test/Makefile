# need to test the kernel string funcs directly rather
# that what gcc decides to optimize with
EXTRA_CFLAGS += -fno-builtin

include  $(BASE_DIR)/../board/AnalogDevices/blackfin/kernel-module.mk
obj-m := string_test.o

