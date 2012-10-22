#############################################################
#
# ptpd2
#
#############################################################
PTPD2_VERSION = HEAD
PTPD2_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/ptpd2
PTPD2_SITE_METHOD = svn

define PTPD2_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -Dlinux -DUSE_LINUX_PHC" LDFLAGS="$(TARGET_LDFLAGS) -lm -lrt" -C $(@D)/src
endef

define PTPD2_INSTALL_TARGET_CMDS
	cp -dpf $(@D)/src/ptpd2 $(TARGET_DIR)/usr/sbin
endef

$(eval $(generic-package))
