FWUPGRADE_SITE = git://gitorious.org/embedded-linux-firmware-upgrade-tool/embedded-linux-firmware-upgrade-tool.git
FWUPGRADE_VERSION = b0cc4fd10a49c9a9fd86bec2dd61f70074b6577e

define FWUPGRADE_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" HOSTCC="$(HOSTCC)" -C $(@D)
endef

define FWUPGRADE_INSTALL_TARGET_CMDS
	install -D -m 0755 $(@D)/fwupgrade-cgi $(TARGET_DIR)/usr/bin/fwupgrade-cgi
# Cheat a little bit by installing a host program inside the target
# installation commands
	install -D -m 0755 $(@D)/fwupgrade-tool $(HOST_DIR)/usr/bin/fwupgrade-tool
endef

$(eval $(call GENTARGETS,package/getinge,fwupgrade))
