IPSETD_SOURCE =
IPSETD_VERSION = 0.1

define IPSETD_BUILD_CMDS
	(cd $(@D); \
		$(TARGET_CC) $(TARGET_CFLAGS) -o ipsetd ipsetd.c)
endef

define IPSETD_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/ipsetd $(TARGET_DIR)/usr/sbin/ipsetd
	$(INSTALL) -D $(@D)/S11ipsetd $(TARGET_DIR)/etc/init.d/S11ipsetd
endef

define IPSETD_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/ipsetd
	rm -f $(TARGET_DIR)/etc/init.d/S11ipsetd
endef

$(eval $(call GENTARGETS,package/getinge,ipsetd))

$(BUILD_DIR)/ipsetd-$(IPSETD_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)mkdir -p $(@D)
	$(Q)cp $($(PKG)_DIR_PREFIX)/ipsetd/src/* $(@D)
	$(Q)touch $@
