FWUPGRADE_SITE = git://gitorious.org/embedded-linux-firmware-upgrade-tool/embedded-linux-firmware-upgrade-tool.git
FWUPGRADE_VERSION = 2ec98aa75619b8978456ad938481c4604c0a3880

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
