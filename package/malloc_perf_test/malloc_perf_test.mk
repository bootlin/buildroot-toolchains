#############################################################
#
# malloc_perf test
#
#############################################################
MALLOC_PERF_TEST_SITE = http://svn.code.sf.net/p/adi-openapp/code/trunk/tests/malloc-perf
MALLOC_PERF_TEST_SITE_METHOD = svn
MALLOC_PERF_TEST_VERSION = HEAD

define MALLOC_PERF_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define MALLOC_PERF_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/malloc-perf $(TARGET_DIR)/bin/
endef

define MALLOC_PERF_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define MALLOC_PERF_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/malloc-perf
endef

$(eval $(generic-package))
