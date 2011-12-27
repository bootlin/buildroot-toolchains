#############################################################
#
# linphone
#
#############################################################

LINPHONE_VERSION:=3.4.0
LINPHONE_SITE:=http://download-mirror.savannah.gnu.org/releases/linphone/3.4.x/sources/
LINPHONE_SOURCE:=linphone-$(LINPHONE_VERSION).tar.gz
LINPHONE_CONF_ENV:=CFLAGS="$(TARGET_CFLAGS) -fno-strict-aliasing -ffast-math -mfast-fp -Wl,--defsym,__stacksize=0x40000"
LINPHONE_CONF_OPT:=--without-crypto \
	--enable-portaudio=no \
	--enable-gtk_ui=no \
	--disable-manual \
	--disable-strict \
	--disable-glib \
	--enable-ipv6=no \
	--disable-shared \
	--enable-static \
	--with-thread-stack-size=0xa000

LINPHONE_DEPENDENCIES:=speex

$(eval $(call AUTOTARGETS,package,linphone))
