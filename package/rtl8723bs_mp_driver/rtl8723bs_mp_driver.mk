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

define RTL8723BS_MP_DRIVER_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) CONFIG_MP_INCLUDED=y KSRC=$(LINUX_DIR) modules
endef

define RTL8723BS_MP_DRIVER_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KSRC=$(LINUX_DIR) \
		PREFIX=$(TARGET_DIR) modules_install
	$(HOST_DIR)/sbin/depmod -a -b $(TARGET_DIR) $(LINUX_VERSION_PROBED)
	$(INSTALL) -D -m 0644 package/rtl8723bs_mp_driver/rtl8723bs_mp.conf \
		$(TARGET_DIR)/etc/modprobe.d/rtl8723bs_mp.conf
endef

$(eval $(generic-package))
