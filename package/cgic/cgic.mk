CGIC_VERSION = 2.05
CGIC_SOURCE  = cgic205.tar.gz
CGIC_SITE    = http://www.boutell.com/cgic/

CGIC_INSTALL_STAGING = YES

define CGIC_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)
endef

define CGIC_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) PREFIX=/usr DESTDIR=$(TARGET_DIR) install
endef

define CGIC_INSTALL_STAGING_CMDS
	$(MAKE) -C $(@D) PREFIX=/usr DESTDIR=$(STAGING_DIR) install
endef

$(eval $(call GENTARGETS,package,cgic))
