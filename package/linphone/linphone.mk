#############################################################
#
# linphone
#
#############################################################
LINPHONE_VERSION = 3.3.2
LINPHONE_SITE = http://download-mirror.savannah.gnu.org/releases/linphone/3.3.x/sources/
LINPHONE_CONF_OPT = \
	--enable-fast-install

LINPHONE_DEPENDENCIES = host-pkgconf ortp mediastreamer libeXosip2 speex

ifeq ($(BR2_bfin), y)
LINPHONE_FLAT_STACKSIZE=0x40000
LINPHONE_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -fno-strict-aliasing -ffast-math -mfast-fp"
LINPHONE_CONF_OPT += \
	--without-crypto \
	--enable-portaudio=no \
	--enable-gtk_ui=no \
	--disable-manual \
	--disable-strict \
	--disable-glib \
	--disable-video \
	--enable-ipv6=no \
	--disable-shared \
	--enable-static \
	--with-thread-stack-size=0xa000

LINPHONE_DEPENDENCIES += libbfgdots
endif

ifeq ($(BR2_PACKAGE_LIBGTK2)$(BR2_PACKAGE_XORG7),yy)
LINPHONE_CONF_OPT += --enable-gtk_ui
LINPHONE_DEPENDENCIES += libgtk2
else
LINPHONE_CONF_OPT += --disable-gtk_ui
endif

$(eval $(autotools-package))
