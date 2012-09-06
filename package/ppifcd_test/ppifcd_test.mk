#############################################################
#
# ppifcd test
#
#############################################################
PPIFCD_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/ppifcd-test
PPIFCD_TEST_SITE_METHOD = svn
PPIFCD_TEST_VERSION = HEAD


define PPIFCD_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS) 
endef

define PPIFCD_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/ppifcd_test $(TARGET_DIR)/bin/
endef

define PPIFCD_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define PPIFCD_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/ppifcd_test
endef

$(eval $(call GENTARGETS))
