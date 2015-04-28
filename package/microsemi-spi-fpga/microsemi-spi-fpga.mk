################################################################################
#
# microsemi-spi-fpga
#
################################################################################

MICROSEMI_SPI_FPGA_SITE = git@tau.free-electrons.com:eca/tools.git
MICROSEMI_SPI_FPGA_VERSION = 71934ae87e855f2b84338604f0e3b92971b3d62f
MICROSEMI_SPI_FPGA_SITE_METHOD = git

MICROSEMI_SPI_FPGA_CONFIGURE_OPTS = \
	CROSS_COMPILE=$(TARGET_CROSS)

define MICROSEMI_SPI_FPGA_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/fpga \
		$(MICROSEMI_SPI_FPGA_CONFIGURE_OPTS)
endef

define MICROSEMI_SPI_FPGA_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/fpga/fpga $(TARGET_DIR)/usr/bin/fpga
endef

$(eval $(generic-package))
