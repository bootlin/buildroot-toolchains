#############################################################
#
# version
#
#############################################################
VERSION_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/version
VERSION_SITE_METHOD = svn
VERSION_VERSION = 10832

define VERSION_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define VERSION_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/version $(TARGET_DIR)/bin/
endef

define VERSION_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define VERSION_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/version
endef

$(eval $(generic-package))

