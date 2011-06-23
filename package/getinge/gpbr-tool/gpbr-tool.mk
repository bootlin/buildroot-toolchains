
GPBR_TOOL_SOURCE =
GPBR_TOOL_VERSION = 0.1

define GPBR_TOOL_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)
endef

define GPBR_TOOL_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/gpbr-tool $(TARGET_DIR)/usr/bin/gpbr-tool
endef

define GPBR_TOOL_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/bin/gpbr-tool
endef

$(eval $(call GENTARGETS,package/getinge,gpbr-tool))

$(BUILD_DIR)/gpbr-tool-$(GPBR_TOOL_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)mkdir -p $(@D)
	$(Q)cp $($(PKG)_DIR_PREFIX)/gpbr-tool/src/* $(@D)
	$(Q)touch $@