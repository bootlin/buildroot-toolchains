#############################################################
#
# edn test
#
#############################################################
EDN_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-apps/edn
EDN_SITE_METHOD = svn
EDN_VERSION = 10617

define EDN_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define EDN_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/edn $(TARGET_DIR)/bin/
endef

define EDN_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define EDN_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/edn
endef

$(eval $(call GENTARGETS,package,edn))
