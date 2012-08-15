#############################################################
#
# libmcapi
#
#############################################################

LIBMCAPI_VERSION = HEAD
LIBMCAPI_SITE_METHOD = svn
LIBMCAPI_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/lib/libmcapi

LIBMCAPI_SUBDIR = libmcapi-2.0
LIBMCAPI_INSTALL_STAGING = YES

KERNEL_DIR = $(TOPDIR)/$(LINUX_SRCDIR)
ifeq ($(BR2_PACKAGE_ICC_CPU_BF609),y)
ICC_MACHINE=bf609
else
ICC_MACHINE=bf561
endif

define LIBMCAPI_AUTOCONF
	cd $(LIBMCAPI_DIR)/$(LIBMCAPI_SUBDIR) && ./autogen.sh
endef

LIBMCAPI_PRE_CONFIGURE_HOOKS+=LIBMCAPI_AUTOCONF
ICC_CONF_OPT="CFLAGS=-I$(KERNEL_DIR)/drivers/staging/icc/include -I$(KERNEL_DIR)/arch/blackfin/include"

LIBMCAPI_CONF_OPT+=$(ICC_CONF_OPT)

$(eval $(call AUTOTARGETS))
