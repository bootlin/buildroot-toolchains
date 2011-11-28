#############################################################
#
# event test
#
#############################################################
EVENT_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/event_test
EVENT_TEST_SITE_METHOD = svn
EVENT_TEST_VERSION = 10459

define EVENT_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define EVENT_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/event_test $(TARGET_DIR)/bin/
endef

define EVENT_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define EVENT_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/event_test
endef

$(eval $(call GENTARGETS,package,event_test))

