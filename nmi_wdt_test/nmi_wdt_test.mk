#############################################################
#
# nmi_wdt test
#
#############################################################
NMI_WDT_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/nmi_wdt_test
NMI_WDT_TEST_SITE_METHOD = svn
NMI_WDT_TEST_VERSION = HEAD

define NMI_WDT_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) MODULE_DIR=$(BASE_DIR)/build/nmi_wdt_test-$(NMI_WDT_TEST_VERSION)
endef

define NMI_WDT_TEST_INSTALL_TARGET_CMDS
        cp -a $(@D)/nmi_wdt_test.ko $(TARGET_DIR)/lib/modules
endef

define NMI_WDT_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) clean
endef

$(BUILD_DIR)/nmi_wdt_test-$(NMI_WDT_TEST_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)cd $(@D)/../
	$(Q)$(TAR) zxf $(DL_DIR)/$($(PKG)_SOURCE) -C $(@D)/../ 
	$(Q)cp $($(PKG)_DIR_PREFIX)/nmi_wdt_test/Makefile $(@D)/
	$(Q)touch $@

$(eval $(generic-package))

