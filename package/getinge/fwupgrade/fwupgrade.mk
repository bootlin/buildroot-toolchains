FWUPGRADE_SITE = git://gitorious.org/embedded-linux-firmware-upgrade-tool/embedded-linux-firmware-upgrade-tool.git
FWUPGRADE_VERSION = a1ffb7a6e61808dc93256c28787a3a0d04715d6b

define FWUPGRADE_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" HOSTCC="$(HOSTCC)" -C $(@D)
endef

define FWUPGRADE_INSTALL_TARGET_CMDS
	install -D -m 0755 $(@D)/fwupgrade $(TARGET_DIR)/usr/bin/fwupgrade
# Cheat a little bit by installing a host program inside the target
# installation commands
	install -D -m 0755 $(@D)/fwupgrade-tool $(HOST_DIR)/usr/bin/fwupgrade-tool
endef

$(eval $(call GENTARGETS,package/getinge,fwupgrade))
