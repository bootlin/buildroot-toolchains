#############################################################
#
# expat
#
#############################################################

EXPAT_VERSION = 2.0.1
EXPAT_SOURCE = expat-$(EXPAT_VERSION).tar.gz
EXPAT_SITE = http://$(BR2_SOURCEFORGE_MIRROR).dl.sourceforge.net/sourceforge/expat
EXPAT_INSTALL_STAGING = YES
ifeq ($(BR2_ABI_FLAT),y)
EXPAT_INSTALL_TARGET = NO
else
EXPAT_INSTALL_TARGET = YES
EXPAT_INSTALL_STAGING_OPT = DESTDIR=$(STAGING_DIR) installlib
EXPAT_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR) installlib
endif

EXPAT_CONF_OPT = --enable-shared

EXPAT_DEPENDENCIES = host-pkg-config

ifeq ($(BR2_ABI_FLAT),y)
define EXPAT_INSTALL_STAGING_CMDS
	install -D -m 755 $(@D)/.libs/libexpat.a $(STAGING_DIR)/usr/lib/libexpat.a
endef
endif


$(eval $(call AUTOTARGETS,package,expat))
$(eval $(call AUTOTARGETS,package,expat,host))
