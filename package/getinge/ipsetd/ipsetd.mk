IPSETD_SOURCE =
IPSETD_SITE   = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/apps/ipsetd-R1_2_2/

define IPSETD_EXTRACT_CMDS
	cp $(IPSETD_SITE)/* $(@D)/
endef

define IPSETD_BUILD_CMDS
	(cd $(@D); \
		$(TARGET_CC) $(TARGET_CFLAGS) -o ipsetd ipsetd.c)
endef

define IPSETD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/ipsetd $(TARGET_DIR)/usr/sbin/ipsetd
	$(INSTALL) -m 0755 -D package/getinge/ipsetd/S11ipsetd $(TARGET_DIR)/etc/init.d/S11ipsetd
endef

define IPSETD_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/ipsetd
	rm -f $(TARGET_DIR)/etc/init.d/S11ipsetd
endef

$(eval $(call GENTARGETS,package/getinge,ipsetd))
