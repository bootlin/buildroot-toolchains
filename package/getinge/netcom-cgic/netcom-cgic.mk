NETCOM_CGIC_SOURCE =
NETCOM_CGIC_INSTALL_STAGING = YES

define NETCOM_CGIC_EXTRACT_CMDS
	cp -a $(GETINGE_LIBS_TOPDIR)/libcgic_201/* $(@D)/
endef

define NETCOM_CGIC_BUILD_CMDS
	$(MAKE) -C $(@D) BUILDROOT=1 clean
	$(MAKE) -C $(@D) BUILDROOT=1 CC="$(TARGET_CC)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)"
endef

define NETCOM_CGIC_INSTALL_STAGING_CMDS
	install -D -m 0644 $(@D)/cgic.h $(STAGING_DIR)/usr/include/cgic.h
	install -D -m 0644 $(@D)/libcgic.a $(STAGING_DIR)/usr/lib/libcgic.a
endef

$(eval $(call GENTARGETS,package/getinge,netcom-cgic))