NUTTX_VERSION = v0.2
NUTTX_SITE = https://github.com/vxmdesign/yartos.git
NUTTX_SITE_METHOD = git
NUTTX_LICENSE = GPLv2+

NUTTX_DEPENDENCIES = host-gcc-final



define NUTTX_CONFIGURE_CMDS
	$(MAKE) -C $(@D) TARGET_BOARD=$(BR2_NUTTX_TARGET_BOARD) $(BR2_NUTTX_DEFCONFIG) 
endef

define NUTTX_BUILD_CMDS
	$(MAKE) -C $(@D) CROSSDEV=$(TARGET_CROSS) ARCROSSDEV=$(TARGET_CROSS) TOPDIR=$(@D)
endef

define NUTTX_INSTALL_TARGET_CMDS
	echo $(BINARIES_DIR)
	install $(@D)/nuttx $(BINARIES_DIR)/nuttx 
endef



nuttx_install_image: 
	make -C $(BUILD_DIR)/nuttx-$(NUTTX_VERSION) V=2 CROSSDEV=$(TARGET_CROSS) ARCROSSDEV=$(TARGET_CROSS) ROMFS=$(BINARIES_DIR)/rootfs.romfs TOPDIR=$(BUILD_DIR)/nuttx-$(NUTTX_VERSION)
	install $(BUILD_DIR)/nuttx-$(NUTTX_VERSION)/nuttx $(BINARIES_DIR)/yartos

ifeq ($(BR2_NUTTX_FSIMAGE),y)

TARGETS_POST += nuttx_install_image

endif

$(eval $(generic-package))



