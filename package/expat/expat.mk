#############################################################
#
# expat
#
#############################################################

EXPAT_VERSION = 2.1.0
EXPAT_SITE = http://$(BR2_SOURCEFORGE_MIRROR).dl.sourceforge.net/project/expat/expat/$(EXPAT_VERSION)
EXPAT_INSTALL_STAGING = YES
EXPAT_DEPENDENCIES = host-pkg-config

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
EXPAT_CONF_OPT = --enable-shared
endif

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
