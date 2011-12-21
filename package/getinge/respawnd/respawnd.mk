RESPAWND_SOURCE =
RESPAWND_SITE   = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/apps/sys-utils/respawnd-R1_3_0/

define RESPAWND_EXTRACT_CMDS
	cp -a $(RESPAWND_SITE)/* $(@D)/
endef

define RESPAWND_BUILD_CMDS
	(cd $(@D); \
		$(TARGET_CC) $(TARGET_CFLAGS) -o respawnd respawnd.c common.c)
endef

define RESPAWND_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/respawnd $(TARGET_DIR)/sbin/respawnd
	(cd $(TARGET_DIR)/sbin;		\
		ln -sf respawnd respawn-on;	\
		ln -sf respawnd respawn-off)
	$(INSTALL) -D -m 0755 package/getinge/respawnd/S12respawnd $(TARGET_DIR)/etc/init.d/S12respawnd
endef

define RESPAWND_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/sbin/respawnd
	rm -f $(TARGET_DIR)/sbin/respawn-on
	rm -f $(TARGET_DIR)/sbin/respawn-off
	rm -f $(TARGET_DIR)/etc/init.d/S12respawnd
endef

$(eval $(call GENTARGETS,package/getinge,respawnd))
