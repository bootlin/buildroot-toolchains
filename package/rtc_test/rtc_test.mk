#############################################################
#
# rtc test
#
#############################################################
RTC_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/rtc-test
RTC_TEST_SITE_METHOD = svn
RTC_TEST_VERSION = 10832

define RTC_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define RTC_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rtc_test $(TARGET_DIR)/bin/
endef

define RTC_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define RTC_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/rtc_test
endef

$(eval $(generic-package))
