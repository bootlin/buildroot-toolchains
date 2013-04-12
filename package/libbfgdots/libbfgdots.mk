#############################################################
#
# libbfgdots
#
#############################################################
LIBBFGDOTS_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/lib/libbfgdots/g729
LIBBFGDOTS_SITE_METHOD = svn
LIBBFGDOTS_VERSION = HEAD
LIBBFGDOTS_INSTALL_STAGING = YES
LIBBFGDOTS_INSTALL_TARGET = YES

define LIBBFGDOTS_BUILD_CMDS
	$(MAKE) -C $(@D)
endef

define LIBBFGDOTS_INSTALL_STAGING_CMDS
	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR) CONFIG_FMT_USE_FDPIC_ELF=$(BR2_BINFMT_FDPIC) install
endef

define LIBBFGDOTS_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) CONFIG_FMT_USE_FDPIC_ELF=$(BR2_BINFMT_FDPIC) install
endef

define LIBBFGDOTS_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(generic-package))

