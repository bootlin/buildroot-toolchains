#############################################################
#
# inet_utils
#
#############################################################
INETUTILS_VERSION = 1.8
INETUTILS_SOURCE = inetutils-$(INETUTILS_VERSION).tar.gz
INETUTILS_SITE = http://ftp.gnu.org/gnu/inetutils/
INETUTILS_AUTORECONF = NO
INETUTILS_INSTALL_STAGING = NO
INETUTILS_INSTALL_TARGET = YES
INETUTILS_CONF_OPT = \
	--disable-ifconfig \

$(eval $(call AUTOTARGETS,package,inetutils))
