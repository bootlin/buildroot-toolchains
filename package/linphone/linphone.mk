#############################################################
#
# linphone
#
#############################################################

LINPHONE_VERSION:=3.1.2
LINPHONE_SITE:=http://download-mirror.savannah.gnu.org/releases/linphone/3.1.x/sources/
LINPHONE_SOURCE:=linphone-$(LINPHONE_VERSION).tar.gz
LINPHONE_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -fno-strict-aliasing -ffast-math -mfast-fp -Wl,--defsym,__stacksize=0x40000"
LINPHONE_CONF_OPT:=--without-crypto \
	--enable-portaudio=no \
	--enable-gtk_ui=no \
	--disable-manual \
	--disable-strict \
	--disable-glib \
	--enable-ipv6=no \
	--disable-shared \
	--enable-static \
	--with-thread-stack-size=0xa000 \
	$(if $(BF2_PACKAGE_LINPHONE_VIDEO),--enable-video,--disable-video)

LINPHONE_DEPENDENCIES:=speex libosip2 libeXosip2 libbfgdots alsa-lib

$(eval $(call AUTOTARGETS,package,linphone))
