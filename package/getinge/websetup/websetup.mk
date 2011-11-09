
WEBSETUP_SOURCE =
WEBSETUP_SITE = $(GETINGE_APPS_TOPDIR)/websetup

define WEBSETUP_EXTRACT_CMDS
	cp -a $(WEBSETUP_SITE)/* $(@D)
endef

define WEBSETUP_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_CMDS)
endef

define WEBSETUP_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_CMDS) prefix=$(TARGET_DIR)/usr install
endef

$(eval $(call GENTARGETS,package/getinge,websetup))
