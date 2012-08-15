#############################################################
#
# ptpd
#
#############################################################
PTPD_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/ptpd
PTPD_SITE_METHOD = svn
PTPD_VERSION = 10459

define PTPD_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" -C $(@D)/src
endef

define PTPD_INSTALL_TARGET_CMDS
	cp -dpf $(@D)/src/ptpd $(TARGET_DIR)/usr/sbin
#	install -m 755 package/ptpd/ptpd.sysvinit $(TARGET_DIR)/etc/init.d/S65ptp
endef

$(eval $(call GENTARGETS))
