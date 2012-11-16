#############################################################
#
# corebld test
#
#############################################################
COREBLD_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-apps/corebld
COREBLD_SITE_METHOD = svn
COREBLD_VERSION = HEAD

define COREBLD_BUILD_CMDS
        $(MAKE) -C $(@D) BASE_DIR=$(BASE_DIR)  MODULE_DIR=$(BASE_DIR)/build/corebld-$(COREBLD_VERSION)/test_module  $(TARGET_CONFIGURE_OPTS)
endef

define COREBLD_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/corebld $(TARGET_DIR)/bin/
	cp -a $(@D)/test_module/dualcore_test.ko $(TARGET_DIR)/lib/modules
endef

define COREBLD_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define COREBLD_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/corebld $(TARGET_DIR)/lib/modules/dualcoreb_test.ko
endef

$(BUILD_DIR)/corebld-$(COREBLD_VERSION)/.stamp_extracted:
	@$(call MESSAGE,"Extracting")
	$(Q)cd $(@D)/../
	$(Q)$(TAR) zxf $(DL_DIR)/$($(PKG)_SOURCE) -C $(@D)/../ 
	$(Q)cp $($(PKG)_DIR_PREFIX)/corebld/Makefile $(@D)/test_module
	$(Q)touch $@

$(eval $(generic-package))

