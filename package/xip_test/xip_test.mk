#############################################################
#
# xip test
#
#############################################################
XIP_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/xip_test
XIP_TEST_SITE_METHOD = svn
XIP_TEST_VERSION = HEAD

define XIP_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define XIP_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/xip_test $(TARGET_DIR)/bin/
endef

define XIP_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define XIP_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/xip_test
endef

$(eval $(call GENTARGETS,package,xip_test))
