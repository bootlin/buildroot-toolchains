################################################################################
#
# openocd
#
################################################################################

OPENOCD_VERSION = 0.8.0
OPENOCD_SOURCE = openocd-$(OPENOCD_VERSION).tar.bz2
OPENOCD_SITE = http://downloads.sourceforge.net/project/openocd/openocd/$(OPENOCD_VERSION)
OPENOCD_LICENSE = GPLv2+
OPENOCD_LICENSE_FILES = COPYING

OPENOCD_AUTORECONF = YES
OPENOCD_CONF_OPTS = \
	--oldincludedir=$(STAGING_DIR)/usr/include \
	--includedir=$(STAGING_DIR)/usr/include \
	--disable-doxygen-html \
	--enable-dummy

OPENOCD_DEPENDENCIES = libusb-compat

# Adapters
ifeq ($(BR2_PACKAGE_OPENOCD_FT2XXX),y)
OPENOCD_CONF_OPTS += --enable-ftdi
OPENOCD_DEPENDENCIES += libftdi
endif


HOST_OPENOCD_DEPENDENCIES = host-libusb-compat host-libftdi

#HOST_OPENOCD_CONF_OPTS = 	\
#	--disable-doxygen-html 	\
#	--enable-dummy 		\
#	--enable-ftdi	

HOST_OPENOCD_CONF_OPTS = --enable-ftdi --disable-doxygen-html

define HOST_OPENOCD_CONFIGURE_CMDS
	(cd $(@D); \
		LDFLAGS="$(HOST_LDFLAGS)" \
		CFLAGS="$(HOST_CFLAGS)" \
		./configure \
			--prefix=$(HOST_DIR)/usr \
			--sysconfdir=$(HOST_DIR)/etc \
			--localstatedir=$(HOST_DIR)/var \
			--disable-gtk-doc \
			--disable-doc \
			--disable-docs \
			--disable-documentation \
			--disable-debug \
			--with-xmlto=no \
			--with-fop=no \
			--disable-dependency-tracking \
			$(HOST_OPENOCD_CONF_OPTS) \
	)
endef



$(eval $(autotools-package))
$(eval $(host-autotools-package))
