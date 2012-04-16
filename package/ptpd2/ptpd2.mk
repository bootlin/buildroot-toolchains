#############################################################
#
# ptpd2
#
#############################################################
PTPD2_VERSION = HEAD
PTPD2_SITE = https://github.com/richardcochran/ptpd-phc.git
PTPD2_SITE_METHOD = git

define PTPD2_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -Dlinux" LDFLAGS="$(TARGET_LDFLAGS) -lm -lrt" USE_LINUX_PHC=n -C $(@D)/src
endef

define PTPD2_INSTALL_TARGET_CMDS
	cp -dpf $(@D)/src/ptpd2 $(TARGET_DIR)/usr/sbin
endef

$(eval $(call GENTARGETS,package,ptpd2))
