#############################################################
#
# expat
#
#############################################################

EXPAT_VERSION = 2.1.0
EXPAT_SITE = http://downloads.sourceforge.net/project/expat/expat/$(EXPAT_VERSION)
EXPAT_INSTALL_STAGING = YES

ifeq ($(BR2_PREFER_STATIC_LIB),y)
EXPAT_INSTALL_TARGET = NO

define EXPAT_INSTALL_STAGING_CMDS
	install -D -m 755 $(@D)/.libs/libexpat.a $(STAGING_DIR)/usr/lib/libexpat.a
	install -D -m 755 $(@D)/lib/expat.h $(STAGING_DIR)/usr/include/expat.h
	install -D -m 755 $(@D)/lib/expat_external.h $(STAGING_DIR)/usr/include/expat_external.h
endef
else
EXPAT_INSTALL_TARGET = YES
EXPAT_INSTALL_STAGING_OPT = DESTDIR=$(STAGING_DIR) installlib
EXPAT_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR) installlib
endif

EXPAT_DEPENDENCIES = host-pkgconf
EXPAT_LICENSE = MIT
EXPAT_LICENSE_FILES = COPYING

$(eval $(autotools-package))
$(eval $(host-autotools-package))
