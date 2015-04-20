NUTTX_VERSION = elena
NUTTX_SITE = git@github.com:vxmdesign/yartos.git
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

$(eval $(generic-package))



