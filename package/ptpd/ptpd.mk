#############################################################
#
# ptpd
#
#############################################################
PTPD_VERSION:=1.0.0
PTPD_SOURCE:=ptpd-$(PTPD_VERSION).tar.gz
PTPD_SITE:=http://downloads.sourceforge.net/ptpd

define PTPD_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -DPTPD_DBGV" LDFLAGS="$(TARGET_LDFLAGS)" -C $(@D)/src
endef

define PTPD_INSTALL_TARGET_CMDS
	cp -dpf $(@D)/src/ptpd $(TARGET_DIR)/usr/sbin
#	install -m 755 package/ptpd/ptpd.sysvinit $(TARGET_DIR)/etc/init.d/S65ptp
endef

$(eval $(call GENTARGETS,package,ptpd))
