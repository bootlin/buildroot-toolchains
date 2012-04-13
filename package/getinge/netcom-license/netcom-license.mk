NETCOM_LICENSE_SOURCE =
NETCOM_LICENSE_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/packages/netcom/initscripts/license/

define NETCOM_LICENSE_EXTRACT_CMDS
	cp -a $(NETCOM_LICENSE_SITE)/* $(@D)/
endef

define NETCOM_LICENSE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/license $(TARGET_DIR)/sbin/license
endef

$(eval $(call GENTARGETS,package/getinge,netcom-license))
