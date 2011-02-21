#############################################################
#
# freetype2
#
#############################################################
FREETYPE2_VERSION = 2.4.4
FREETYPE2_SITE = http://$(BR2_SOURCEFORGE_MIRROR).dl.sourceforge.net/sourceforge/freetype
FREETYPE2_SOURCE = freetype-$(FREETYPE2_VERSION).tar.bz2
FREETYPE2_INSTALL_STAGING = YES
FREETYPE2_INSTALL_TARGET = YES
FREETYPE2_MAKE_OPT = CCexe="$(HOSTCC)"
FREETYPE2_DEPENDENCIES = host-pkg-config $(if $(BR2_PACKAGE_ZLIB),zlib)

HOST_FREETYPE2_DEPENDENCIES = host-pkg-config

$(eval $(call AUTOTARGETS,package,freetype2))
$(eval $(call AUTOTARGETS,package,freetype2,host))
