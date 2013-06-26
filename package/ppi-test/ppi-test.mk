#############################################################
#
# bfin ppi test
#
#############################################################
PPI_TEST_SITE = http://svn.code.sf.net/p/adi-openapp/code/trunk/tests/ppi-test
PPI_TEST_SITE_METHOD = svn
PPI_TEST_VERSION = HEAD

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
