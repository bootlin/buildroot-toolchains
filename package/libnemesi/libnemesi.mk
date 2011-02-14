#############################################################
#
# Libnemesi (RTSP/RTP client library)
#
#############################################################
LIBNEMESI_VERSION = 0.6
LIBNEMESI_SITE = http://cgit.lscube.org/cgit.cgi/libnemesi/snapshot
LIBNEMESI_SOURCE = libnemesi-$(LIBNEMESI_VERSION).tar.gz
LIBNEMESI_DEPENDENCIES = netembryo
LIBNEMESI_LIBTOOL_PATCH = NO
LIBNEMESI_AUTORECONF = YES
LIBNEMESI_INSTALL_STAGING = YES
LIBNEMESI_INSTALL_TARGET = YES
LIBNEMESI_CONF_OPT = --disable-sctp \

$(eval $(call AUTOTARGETS,package,libnemesi))
