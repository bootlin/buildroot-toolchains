NETCOM_USBRUN_SOURCE =
NETCOM_USBRUN_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/packages/netcom/initscripts/usbrun/

define NETCOM_USBRUN_EXTRACT_CMDS
	cp -a $(NETCOM_USBRUN_SITE)/* $(@D)/
endef

define NETCOM_USBRUN_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rc $(TARGET_DIR)/etc/init.d/S60usbrun
endef

$(eval $(call GENTARGETS,package/getinge,netcom-usbrun))
