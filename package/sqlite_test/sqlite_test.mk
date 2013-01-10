#############################################################
#
# sqlite test
#
#############################################################
SQLITE_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/sqlite_test
SQLITE_TEST_SITE_METHOD = svn
SQLITE_TEST_VERSION = 10832


define SQLITE_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define SQLITE_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/sqlite_test $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/wishlist.sql $(TARGET_DIR)/usr/
endef

define SQLITE_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define SQLITE_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/sqlite_test
	rm -f $(TARGET_DIR)/usr/wishlist.sql
endef

$(eval $(generic-package))
