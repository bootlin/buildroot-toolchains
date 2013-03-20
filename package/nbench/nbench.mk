#############################################################
#
# nbench
#
#############################################################
NBENCH_VERSION:=2.2.3
NBENCH_SOURCE:=nbench-byte-$(NBENCH_VERSION).tar.gz
NBENCH_SITE:=http://www.tux.org/~mayer/linux/
NBENCH_FLAT_STACKSIZE=64000

define NBENCH_CONFIGURE_CMDS
	$(call CONFIG_UPDATE, $(@D))
	sed -i 's/CFLAGS=/CFLAGS+=/g' $(@D)/Makefile
	sed -i '/cd .*doc/d' $(@D)/Makefile
	sed -i '/include/d' $(@D)/Makefile
	touch $@
endef

define NBENCH_BUILD_CMDS
	$(MAKE1) CFLAGS="$(TARGET_CFLAGS)" LINKFLAGS="$(TARGET_LDFLAGS) $(NBENCH_FLAT_LDFLAGS)" OS=$(ARCH) CC=$(TARGET_CC) -C $(@D)
endef

define NBENCH_INSTALL_TARGET_CMDS
	$(MAKE) CFLAGS="$(TARGET_CFLAGS)" OS=$(ARCH) CC="$(TARGET_CC)" BASE=$(TARGET_DIR) -C $(@D) install
endef

define NBENCH_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(generic-package))

