#############################################################
#
# nbench
#
#############################################################
NBENCH_VERSION:=2.2.3
NBENCH_SOURCE:=nbench-byte-$(NBENCH_VERSION).tar.gz
NBENCH_SITE:=http://www.tux.org/~mayer/linux/

define NBENCH_CONFIGURE_CMDS
	$(call CONFIG_UPDATE, $(@D))
	sed -i 's/CFLAGS=/CFLAGS+=/g' $(@D)/Makefile
	sed -i '/cd .*doc/d' $(@D)/Makefile
	sed -i '/include/d' $(@D)/Makefile
	touch $@
endef

define NBENCH_BUILD_CMDS
ifeq ($(BR2_ABI_FLAT),y)
	$(MAKE) CFLAGS="$(TARGET_CFLAGS)" LINKFLAGS="$(TARGET_LDFLAGS) -Wl,-elf2flt=-s64000" OS=$(ARCH) CC="$(TARGET_CC)" -C $(@D)
else
	$(MAKE) CFLAGS="$(TARGET_CFLAGS)" OS=$(ARCH) CC="$(TARGET_CC)" -C $(@D)
endif
endef

define NBENCH_INSTALL_TARGET_CMDS
	$(MAKE) CFLAGS="$(TARGET_CFLAGS)" OS=$(ARCH) CC="$(TARGET_CC)" BASE=$(TARGET_DIR) -C $(@D) install
endef

define NBENCH_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call AUTOTARGETS,package,nbench))

