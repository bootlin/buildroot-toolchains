################################################################################
#
# dtc-overlay
#
################################################################################

DTC_OVERLAY_VERSION = 61bbb7e7719959dc70917ae855398d278afa99c7
DTC_OVERLAY_SITE = $(call github,atenart,dtc,$(DTC_OVERLAY_VERSION))
DTC_OVERLAY_LICENSE = GPLv2+/BSD-2c
DTC_OVERLAY_LICENSE_FILES = README.license GPL
DTC_OVERLAY_DEPENDENCIES = host-bison host-flex

####

define HOST_DTC_OVERLAY_BUILD_CMDS
	$(HOST_CONFIGURE_OPTS) $(MAKE) -C $(@D) PREFIX=/usr
endef

define HOST_DTC_OVERLAY_INSTALL_CMDS
	$(MAKE) -C $(@D) DESTDIR=$(HOST_DIR) PREFIX=/usr install-bin
endef

$(eval $(host-generic-package))
