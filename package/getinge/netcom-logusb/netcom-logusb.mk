NETCOM_LOGUSB_SOURCE =
NETCOM_LOGUSB_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/packages/netcom/initscripts/logusb/

define NETCOM_LOGUSB_EXTRACT_CMDS
	cp -a $(NETCOM_LOGUSB_SITE)/* $(@D)/
endef

define NETCOM_LOGUSB_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/logusb $(TARGET_DIR)/sbin/logusb
endef

$(eval $(call GENTARGETS,package/getinge,netcom-logusb))
