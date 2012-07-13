#############################################################
#
# sram_alloc test
#
#############################################################
SRAM_ALLOC_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/sram-alloc-test
SRAM_ALLOC_TEST_SITE_METHOD = svn
SRAM_ALLOC_TEST_VERSION = 10736

define SRAM_ALLOC_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define SRAM_ALLOC_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/sram_alloc $(TARGET_DIR)/bin/
endef

define SRAM_ALLOC_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define SRAM_ALLOC_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/sram_alloc
endef

$(eval $(call GENTARGETS,package,sram_alloc_test))
