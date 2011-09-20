
SERSRVD_SOURCE  =
SERSRVD_VERSION = 1.0

define SERSRVD_BUILD_CMDS
	$(MAKE) -C $(@D)/src CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)"
endef

define SERSRVD_INSTALL_TARGET_CMDS
	install -D -m 0755 $(@D)/src/sersrvd $(TARGET_DIR)/usr/sbin/sersrvd
	install -D -m 0644 $(@D)/src/sersrvd.conf $(TARGET_DIR)/etc/sersrvd.conf
endef

$(eval $(call GENTARGETS,package/getinge,sersrvd))

$(BUILD_DIR)/sersrvd-$(SERSRVD_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)mkdir -p $(@D)
	$(Q)cp -a $($(PKG)_DIR_PREFIX)/sersrvd/src/* $(@D)
	$(Q)touch $@
