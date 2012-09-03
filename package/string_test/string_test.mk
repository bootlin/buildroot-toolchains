#############################################################
#
# string test
#
#############################################################
STRING_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/string_test
STRING_TEST_SITE_METHOD = svn
STRING_TEST_VERSION = 10630

define STRING_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) MODULE_DIR=$(BASE_DIR)/build/string_test-$(STRING_TEST_VERSION)
endef

define STRING_TEST_INSTALL_TARGET_CMDS
        cp -a $(@D)/string_test.ko $(TARGET_DIR)/lib/modules
endef

define STRING_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) clean
endef

$(BUILD_DIR)/string_test-$(STRING_TEST_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)cd $(@D)/../
	$(Q)$(TAR) zxf $(DL_DIR)/$($(PKG)_SOURCE) -C $(@D)/../ 
	$(Q)cp $($(PKG)_DIR_PREFIX)/string_test/Makefile $(@D)/
	$(Q)touch $@

$(eval $(generic-package))

