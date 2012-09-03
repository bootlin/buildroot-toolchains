#############################################################
#
# linphone
#
#############################################################
LINPHONE_VERSION = 3.5.2
LINPHONE_SITE = http://download-mirror.savannah.gnu.org/releases/linphone/3.5.x/sources/
LINPHONE_CONF_OPT = \
	--enable-external-ortp \
	--enable-external-mediastreamer

LINPHONE_DEPENDENCIES = host-pkg-config ortp mediastreamer libeXosip2 speex

ifeq ($(BR2_BFIN), y)
LINPHONE_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -fno-strict-aliasing -ffast-math -mfast-fp -Wl,--defsym,__stacksize=0x40000"
LINPHONE_CONF_OPT += \
	--without-crypto \
	--enable-external-ortp \
	--enable-external-mediastreamer \
	--without-crypto \
	--enable-portaudio=no \
	--enable-gtk_ui=no \
	--disable-manual \
	--disable-strict \
	--disable-glib \
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
