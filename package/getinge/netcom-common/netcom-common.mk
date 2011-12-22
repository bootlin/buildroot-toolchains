NETCOM_COMMON_SOURCE =
NETCOM_COMMON_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/packages/netcom/common/

define NETCOM_COMMON_EXTRACT_CMDS
	cp -a $(NETCOM_COMMON_SITE)/* $(@D)/
endef

define NETCOM_COMMON_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/functions $(TARGET_DIR)/usr/share/netcom/functions
endef

$(eval $(call GENTARGETS,package/getinge,netcom-common))
