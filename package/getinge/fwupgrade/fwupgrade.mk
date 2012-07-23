FWUPGRADE_SITE = git://gitorious.org/embedded-linux-firmware-upgrade-tool/embedded-linux-firmware-upgrade-tool.git
FWUPGRADE_VERSION = 067fa970454bb0a2f0c9579904fae8b4b6639a76

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
