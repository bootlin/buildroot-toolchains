NETCOM_REGISTER_SOURCE =
NETCOM_REGISTER_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/packages/netcom/initscripts/register/

define NETCOM_REGISTER_EXTRACT_CMDS
	cp -a $(NETCOM_REGISTER_SITE)/* $(@D)/
endef

define NETCOM_REGISTER_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/register $(TARGET_DIR)/sbin/register
endef

$(eval $(call GENTARGETS,package/getinge,netcom-register))
