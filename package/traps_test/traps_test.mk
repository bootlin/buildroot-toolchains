#############################################################
#
# traps test
#
#############################################################
TRAPS_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/crash_test
TRAPS_TEST_SITE_METHOD = svn
TRAPS_TEST_VERSION = HEAD

define TRAPS_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define TRAPS_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/traps_test $(TARGET_DIR)/bin/
endef

define TRAPS_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define TRAPS_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/traps_test
endef

$(eval $(generic-package))
