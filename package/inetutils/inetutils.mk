#############################################################
#
# inet_utils
#
#############################################################
INETUTILS_VERSION = 1.8
INETUTILS_SOURCE = inetutils-$(INETUTILS_VERSION).tar.gz
INETUTILS_SITE = http://ftp.gnu.org/gnu/inetutils/
INETUTILS_INSTALL_STAGING = NO
INETUTILS_INSTALL_TARGET_OPT = SUIDMODE="-m 4775" DESTDIR=$(TARGET_DIR) install
INETUTILS_CONF_OPT = \
		     --disable-ifconfig \
		     --without-included-regex \
		     --disable-libls \
		     --disable-servers --disable-clients \
		     --enable-rcp \
		     --enable-rsh \
		     --enable-rshd
INETUTILS_DEPENDENCIES  += host-automake
$(eval $(autotools-package))
