RESPAWND_SOURCE =
RESPAWND_VERSION = 0.1

define RESPAWND_BUILD_CMDS
	(cd $(@D); \
		$(TARGET_CC) $(TARGET_CFLAGS) -o respawnd respawnd.c common.c)
endef

define RESPAWND_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/respawnd $(TARGET_DIR)/sbin/respawnd
	(cd $(TARGET_DIR)/sbin;		\
		ln -sf respawnd respawn-on;	\
		ln -sf respawnd respawn-off)
	$(INSTALL) -D -m 0755 $(@D)/S12respawnd $(TARGET_DIR)/etc/init.d/S12respawnd
endef

define RESPAWND_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/sbin/respawnd
	rm -f $(TARGET_DIR)/sbin/respawn-on
	rm -f $(TARGET_DIR)/sbin/respawn-off
	rm -f $(TARGET_DIR)/etc/init.d/S12respawnd
endef

$(eval $(call GENTARGETS,package/getinge,respawnd))

$(BUILD_DIR)/respawnd-$(RESPAWND_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)mkdir -p $(@D)
	$(Q)cp $($(PKG)_DIR_PREFIX)/respawnd/src/* $(@D)
	$(Q)touch $@
