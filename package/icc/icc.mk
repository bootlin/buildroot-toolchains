#############################################################
#
# bfin icc
#
#############################################################
PKG:=ICC
ICC_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-apps/icc_utils
ICC_SITE_METHOD = svn
ICC_VERSION = HEAD
ICC_DL_VERSION = $(ICC_VERSION)
ICC_BASE_NAME=icc-$(ICC_VERSION)
ICC_DL_DIR=$(DL_DIR)/$(ICC_BASE_NAME)

ICC_DIR:=$(BUILD_DIR)/icc-$(ICC_VERSION)
ICC_SOURCE:=icc-$(ICC_VERSION).tar.gz
ICC_CAT=$(ZCAT)

KERNEL_DIR = $(TOPDIR)/$(LINUX26_SOURCE_DIR)
LIBMCAPI_INCLUDE = $(ICC_DIR)/include

ifeq ($(BR2_PACKAGE_ICC_CPU_BF609),y)
ICC_MACHINE=bf609
else
ICC_MACHINE=bf561
endif

ifeq ($(BR2_PACKAGE_ICC),y)
ICC_BUILDROOT=y
else
ICC_BUILDROOT=n
endif

$(DL_DIR)/$(ICC_SOURCE):
	$(call DOWNLOAD,$(ICC_SITE),$(ICC_SOURCE))

icc-source: $(DL_DIR)/$(ICC_SOURCE)

$(ICC_DIR)/.unpacked: $(DL_DIR)/$(ICC_SOURCE)
	$(ICC_CAT) $(DL_DIR)/$(ICC_SOURCE) | $(TAR) -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(ICC_DIR)/.unpacked

icc-extract: $(ICC_DIR)/.unpacked

define ICC_CORE_BUILD_CMDS
        $(MAKE) -C $(ICC_DIR)/icc_core KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE)
endef

define ICC_LOADER_BUILD_CMDS
	$(MAKE) -C  $(ICC_DIR)/icc_loader KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE)
endef

define ICC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(ICC_DIR)/icc_core/icc  $(TARGET_DIR)/bin/
endef

define ICC_LOADER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(ICC_DIR)/icc_loader/icc_loader  $(TARGET_DIR)/bin/
endef

define ICC_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define ICC_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/icc
endef

icc_core-build:
	ICC_CORE_BUILD_CMDS

icc_core-install:
	ICC_INSTALL_TARGET_CMDS

icc_loader-build:
	ICC_LOADER_BUILD_CMDS

icc_loader-install:
	ICC_LOADER_INSTALL_TARGET_CMDS

libmcapi_coreb-build:
	$(MAKE) -C $(ICC_DIR)/libmcapi_coreb KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE) ICC_BUILDROOT=$(ICC_BUILDROOT) \
		LIBMCAPI_INCLUDE=$(LIBMCAPI_INCLUDE)

libmcapi_coreb-install: libmcapi_coreb-build
	$(MAKE) -C $(ICC_DIR)/libmcapi_coreb DESTDIR=$(STAGING_DIR) install ICC_BUILDROOT=$(ICC_BUILDROOT)

icc_example-build:
	$(MAKE) -C $(ICC_DIR)/example/task  KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE)
	$(MAKE) -C $(ICC_DIR)/example/test_app KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE) 


icc-build: icc_core-build icc_loader-build libmcapi_coreb-build


ifeq ($(BR2_PACKAGE_ICC),y)
TARGETS+=icc
endif

