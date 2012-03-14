#############################################################
#
# mmap test
#
#############################################################
MMAP_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/mmap_test
MMAP_TEST_SITE_METHOD = svn
MMAP_TEST_VERSION = HEAD

define MMAP_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) MODULE_DIR=$(BASE_DIR)/build/mmap_test-$(MMAP_TEST_VERSION) $(TARGET_CONFIGURE_OPTS)
endef

define MMAP_TEST_INSTALL_TARGET_CMDS
        cp -a $(@D)/simple.ko $(TARGET_DIR)/lib/modules
endef

define MMAP_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) clean
endef

$(eval $(call GENTARGETS,package,mmap_test))

$(BUILD_DIR)/mmap_test-$(MMAP_TEST_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)cd $(@D)/../
	$(Q)$(TAR) zxf $(DL_DIR)/$($(PKG)_SOURCE) -C $(@D)/../ 
	$(Q)cp $($(PKG)_DIR_PREFIX)/mmap_test/Makefile $(@D)/
	$(Q)touch $@


