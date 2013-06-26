#############################################################
#
# rtscts test util
#
#############################################################
RTSCTS_UTIL_SITE = http://svn.code.sf.net/p/adi-openapp/code/trunk/tests/rtscts_test
RTSCTS_UTIL_SITE_METHOD = svn
RTSCTS_UTIL_VERSION = HEAD

define RTSCTS_UTIL_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define RTSCTS_UTIL_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rtscts_test $(TARGET_DIR)/bin/
endef

define RTSCTS_UTIL_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define RTSCTS_UTIL_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/rtscts_test
endef

$(eval $(generic-package))

