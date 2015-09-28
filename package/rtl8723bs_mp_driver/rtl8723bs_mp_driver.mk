################################################################################
#
# rtl8723bs MP SDIO WiFi Driver
#
################################################################################

RTL8723BS_MP_DRIVER_VERSION = $(call qstrip,$(BR2_PACKAGE_RTL8723BS_MP_DRIVER_VERSION))
RTL8723BS_MP_DRIVER_SOURCE = rtl8723BS_WiFi_linux_$(RTL8723BS_MP_DRIVER_VERSION).tar.gz
RTL8723BS_MP_DRIVER_SITE = ftp://nowhere
RTL8723BS_MP_DRIVER_SITE_METHOD = ftp

RTL8723BS_MP_DRIVER_DEPENDENCIES = linux
#RTL8723BS_MP_DRIVER_INSTALL_STAGING = YES
RTL8723BS_MP_DRIVER_LICENSE = GPLv2

ifeq ($(BR2_PACKAGE_RTL8723BS_MP_DRIVER_CUSTOM_GIT),y)
RTL8723BS_MP_DRIVER_SITE = $(call qstrip,$(BR2_PACKAGE_RTL8723BS_MP_DRIVER_CUSTOM_REPO_URL))
RTL8723BS_MP_DRIVER_SITE_METHOD = git
BR_NO_CHECK_HASH_FOR += $(RTL8723BS_MP_DRIVER_SOURCE)
endif

RTL8723BS_MAKE_FLAGS = -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
RTL8723BS_MAKE_FLAGS += -DCONFIG_CONCURRENT_MODE
#RTL8723BS_MAKE_FLAGS += -DCONFIG_AP_MODE
RTL8723BS_MAKE_FLAGS += -DCONFIG_AUTO_AP_MODE
RTL8723BS_MAKE_FLAGS += -DCONFIG_MP_INCLUDED

define RTL8723BS_MP_DRIVER_BUILD_CMDS
	USER_EXTRA_CFLAGS="$(RTL8723BS_MAKE_FLAGS)" $(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KSRC=$(LINUX_DIR) modules
endef

define RTL8723BS_MP_DRIVER_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KSRC=$(LINUX_DIR) \
		PREFIX=$(TARGET_DIR) modules_install
	$(HOST_DIR)/sbin/depmod -a -b $(TARGET_DIR) $(LINUX_VERSION_PROBED)
	$(INSTALL) -D -m 0644 package/rtl8723bs_mp_driver/rtl8723bs_mp.conf \
		$(TARGET_DIR)/etc/modprobe.d/rtl8723bs_mp.conf
	$(INSTALL) -m 0755 -D package/rtl8723bs_mp_driver/S20_rtl8723bs_mp_wifi $(TARGET_DIR)/etc/init.d/S20_rtl8723bs_mp_wifi

  $(INSTALL) -D -m 0755 package/rtl8723bs_mp_driver/w_start $(TARGET_DIR)/usr/bin/w_start
  $(INSTALL) -D -m 0755 package/rtl8723bs_mp_driver/w_stop  $(TARGET_DIR)/usr/bin/w_stop
  $(INSTALL) -D -m 0755 package/rtl8723bs_mp_driver/w       $(TARGET_DIR)/usr/bin/w
endef

$(eval $(generic-package))
