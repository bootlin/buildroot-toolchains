NEWLIB_VERSION = 2.2.0
NEWLIB_SITE = ftp://sourceware.org/pub/newlib
NEWLIB_LICENSE = MIT
NEWLIB_LICENSE_FILES = COPYRIGHT

NEWLIB_DEPENDENCIES = host-gcc-initial

NEWLIB_ADD_TOOLCHAIN_DEPENDENCY = NO

NEWLIB_INSTALL_STAGING = YES
#		$(TARGET_CONFIGURE_OPTS) 
define NEWLIB_CONFIGURE_CMDS
	(cd $(@D); \
		$(TARGET_MAKE_ENV) \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_HOST_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=$(STAGING_DIR) \
			--includedir=$(STAGING_DIR)/usr/include \
			--oldincludedir=$(STAGING_DIR)/usr/include \
			--with-build-sysroot=$(STAGING_DIR) \
			--enable-newlib-io-long-long \
			--enable-newlib-register-fini \
			--disable-newlib-supplied-syscalls \
			--disable-nls)

endef

define NEWLIB_APPLY_PATCHES
	$(APPLY_PATCHES) $(@D) package/newlib \*.patch
endef


define NEWLIB_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef
#	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR) install
define NEWLIB_INSTALL_STAGING_CMDS
	mkdir -p $(HOST_DIR)/usr/$(GNU_TARGET_NAME)/lib
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install
endef

#	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) install

#define NEWLIB_INSTALL_TARGET_CMDS
#	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)  DESTDIR=$(TARGET_DIR) REF_NOTE=1 install
#endef

$(eval $(generic-package))

