#############################################################
#
# pthread test
#
#############################################################
PTHREAD_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/pthread_test
PTHREAD_TEST_SITE_METHOD = svn
PTHREAD_TEST_VERSION = 10832


PTH_LDFLAGS = $(TARGET_LDFLAGS) -lpthread

define PTHREAD_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS) LDFLAGS="$(PTH_LDFLAGS)"
endef

define PTHREAD_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/ex? $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/ptest $(TARGET_DIR)/bin/
endef

define PTHREAD_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define PTHREAD_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/ex?
	rm -f $(TARGET_DIR)/bin/ptest
endef

$(eval $(generic-package))
