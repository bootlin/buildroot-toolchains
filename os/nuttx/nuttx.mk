NUTTX_VERSION = master
NUTTX_SITE = git@github.com:vxmdesign/vtos.git
NUTTX_SITE_METHOD = git
NUTTX_LICENSE = GPLv2+

NUTTX_DEPENDENCIES = host-gcc-final

define NUTTX_CONFIGURE_CMDS
	echo "est"
endef

define NUTTX_BUILD_CMDS
	$(Q) $(MAKE) -C $(@D)
endef

define NUTTX_INSTALL_TARGET_CMDS
	echo "tes"
endef

$(eval $(generic-package))



