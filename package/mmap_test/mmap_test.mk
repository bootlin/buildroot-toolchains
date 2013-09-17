#############################################################
#
# mmap test
#
#############################################################
MMAP_TEST_SITE = http://svn.code.sf.net/p/adi-openapp/code/trunk/tests/mmap_test
MMAP_TEST_SITE_METHOD = svn
MMAP_TEST_VERSION = 927
MMAP_TEST_DEPENDENCIES = linux

define MMAP_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) MODULE_DIR=$(BASE_DIR)/build/mmap_test-$(MMAP_TEST_VERSION)
endef

define MMAP_TEST_INSTALL_TARGET_CMDS
        cp -a $(@D)/simple.ko $(TARGET_DIR)/lib/modules
endef

define MMAP_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) clean
endef

$(BUILD_DIR)/mmap_test-$(MMAP_TEST_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)cd $(@D)/../
	$(Q)$(TAR) zxf $(DL_DIR)/$($(PKG)_SOURCE) -C $(@D)/../ 
	$(Q)cp $($(PKG)_DIR_PREFIX)/mmap_test/Makefile $(@D)/
	$(Q)touch $@

$(eval $(generic-package))

