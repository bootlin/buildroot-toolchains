################################################################################
#
# rtl8723bs_bt SDIO Bluetooth Config and Firmware
#
################################################################################

RTL8723BS_BT_VERSION = $(call qstrip,$(BR2_PACKAGE_RTL8723BS_BT_VERSION))

RTL8723BS_BT_DEPENDENCIES = linux
#RTL8723BS_BT_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_RTL8723BS_BT_CUSTOM_GIT),y)
RTL8723BS_BT_SITE = $(call qstrip,$(BR2_PACKAGE_RTL8723BS_BT_CUSTOM_REPO_URL))
RTL8723BS_BT_SITE_METHOD = git
BR_NO_CHECK_HASH_FOR += $(RTL8723BS_BT_SOURCE)
endif

define RTL8723BS_BT_BUILD_CMDS
	$(MAKE) -C $(@D) CC=$(BR2_TOOLCHAIN_EXTERNAL_PREFIX)-gcc
endef

define RTL8723BS_BT_INSTALL_INIT_SYSV
    $(INSTALL) -m 0755 -D package/rtl8723bs_bt/S25rtk_hciattach $(TARGET_DIR)/etc/init.d/S25rtk_hciattach
    $(INSTALL) -m 0755 -D package/rtl8723bs_bt/start_rtl8723bs_bt $(TARGET_DIR)/sbin/start_rtl8723bs_bt
endef

define RTL8723BS_BT_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) CC=$(BR2_TOOLCHAIN_EXTERNAL_PREFIX)-gcc \
		PREFIX=$(TARGET_DIR) install
endef

$(eval $(generic-package))
