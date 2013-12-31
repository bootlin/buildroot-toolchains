#############################################################
#
# bfin common sport test
#
#############################################################
SPORT_TEST_SITE = http://svn.code.sf.net/p/adi-openapp/code/trunk/tests/sport_test
SPORT_TEST_SITE_METHOD = svn
SPORT_TEST_VERSION = 927

define SPORT_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define SPORT_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/sport_test $(TARGET_DIR)/bin/
endef

define SPORT_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define SPORT_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/sport_test
endef

$(eval $(generic-package))
