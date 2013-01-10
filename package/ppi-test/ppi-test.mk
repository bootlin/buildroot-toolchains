#############################################################
#
# bfin ppi test
#
#############################################################
PPI_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/ppi-test
PPI_TEST_SITE_METHOD = svn
PPI_TEST_VERSION = 10832

define PPI_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define PPI_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/ppitest $(TARGET_DIR)/bin/
endef

define PPI_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define PPI_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/ppitest
endef

$(eval $(generic-package))
