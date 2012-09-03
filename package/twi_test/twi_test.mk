#############################################################
#
# twi test
#
#############################################################
TWI_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/twi_test
TWI_TEST_SITE_METHOD = svn
TWI_TEST_VERSION = HEAD

define TWI_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR) MODULE_DIR=$(BASE_DIR)/build/twi_test-$(TWI_TEST_VERSION)  
endef

define TWI_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/twi_test $(TARGET_DIR)/bin/
	cp -a $(@D)/twi_smbus_test.ko $(TARGET_DIR)/lib/modules
endef

define TWI_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define TWI_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/twi_test $(TARGET_DIR)/lib/modules/twi_smbus_test.ko
endef

$(BUILD_DIR)/twi_test-$(TWI_TEST_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)cd $(@D)/../
	$(Q)$(TAR) zxf $(DL_DIR)/$($(PKG)_SOURCE) -C $(@D)/../ 
	$(Q)cp $($(PKG)_DIR_PREFIX)/twi_test/Makefile $(@D)/
	$(Q)touch $@

$(eval $(generic-package))

