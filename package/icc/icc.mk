#############################################################
#
# bfin icc
#
#############################################################
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
ICC_INCLUDE = $(ICC_DIR)/include
MCAPI_INCLUDE=$(STAGING_DIR)/usr/include/mcapi-2.0_coreb/
MCAPI_TEST_INCLUDE=$(STAGING_DIR)/usr/include/mcapi-2.0/
LIBMCAPI_COREB=$(STAGING_DIR)/usr/lib/libmcapi_coreb.a

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

ifeq ($(BR2_ICC_DEBUG),y)
define PATCH_ICC_DEBUG_CMD
	sed -i 's/^\/\* #define.*DEBUG \*\/$$/#define DEBUG/' $(ICC_DIR)/include/debug.h
endef
else
define PATCH_ICC_DEBUG_CMD
	sed -i 's/^#define.*DEBUG$$/\/\* #define DEBUG \*\//' $(ICC_DIR)/include/debug.h
endef
endif

INSTALL_TASK_SRC = $(notdir $(wildcard $(ICC_DIR)/example/task/*.c))
INSTALL_TASK = $(patsubst %.c,%,$(INSTALL_TASK_SRC))

INSTALL_TEST_APP_SRC = $(notdir $(wildcard $(ICC_DIR)/example/test_app/*.c))
INSTALL_TEST_APP = $(patsubst %.c,%_test,$(INSTALL_TEST_APP_SRC))

define ICC_CORE_BUILD_CMDS
        $(MAKE1) -C $(ICC_DIR)/icc_core KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE)
endef

define ICC_LOADER_BUILD_CMDS
	$(MAKE1) -C  $(ICC_DIR)/icc_loader KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE)
endef

define ICC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(ICC_DIR)/icc_core/icc  $(TARGET_DIR)/bin/
endef

define ICC_LOADER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(ICC_DIR)/icc_loader/icc_loader  $(TARGET_DIR)/bin/
endef

define ICC_CLEAN_CMDS
	$(MAKE1) -C $(@D) clean
endef

define ICC_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/icc
endef


$(DL_DIR)/$(ICC_SOURCE):
	$(call DOWNLOAD,$(ICC_SITE),$(ICC_SOURCE))

icc-source: $(DL_DIR)/$(ICC_SOURCE)

$(ICC_DIR)/.stamp_unpacked: $(DL_DIR)/$(ICC_SOURCE) linux
	$(ICC_CAT) $(DL_DIR)/$(ICC_SOURCE) | $(TAR) -C $(BUILD_DIR) $(TAR_OPTIONS) -
	cp -dpfr $(LINUX26_BUILD_DIR)/include/generated $(ICC_DIR)/include/
	cp -dpfr $(LINUX26_BUILD_DIR)/arch/blackfin/include/generated/* $(ICC_DIR)/include/
	$(PATCH_ICC_DEBUG_CMD)
	$(Q)touch $@

$(ICC_DIR)/.stamp_build: icc_core-build icc_loader-build libmcapi_coreb-build  libmcapi
	$(Q)touch $@

$(ICC_DIR)/.stamp_install: icc_core-install icc_loader-install libmcapi_coreb-install icc_task-install
	$(Q)touch $@

$(ICC_DIR)/.stamp_clean: icc_task-uninstall
	$(ICC_UNINSTALL_TARGET_CMDS)
	rm $(ICC_DIR)/.stamp*

icc: icc-install

icc-install: icc-build $(ICC_DIR)/.stamp_install


icc-build: icc-extract  $(ICC_DIR)/.stamp_build 


icc-extract: $(ICC_DIR)/.stamp_unpacked

icc-clean: $(ICC_DIR)/.stamp_clean

icc-distclean: icc-clean
	rm -rf $(ICC_DIR)


icc_core-build: $(ICC_DIR)/.stamp_icc_core-build

$(ICC_DIR)/.stamp_icc_core-build:
	$(ICC_CORE_BUILD_CMDS)
	touch $@

icc_core-install: icc_core-build
	$(ICC_INSTALL_TARGET_CMDS)

icc_loader-build: $(ICC_DIR)/.stamp_icc_loader-build

$(ICC_DIR)/.stamp_icc_loader-build:
	$(ICC_LOADER_BUILD_CMDS)
	touch $@

icc_loader-install: icc_loader-build
	$(ICC_LOADER_INSTALL_TARGET_CMDS)

libmcapi_coreb-build: $(ICC_DIR)/.stamp_libmcapi_coreb-build

$(ICC_DIR)/.stamp_libmcapi_coreb-build:
	$(MAKE1) -C $(ICC_DIR)/libmcapi_coreb KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE) ICC_BUILDROOT=$(ICC_BUILDROOT) \
		ICC_INCLUDE=$(ICC_INCLUDE) build
	touch $@

libmcapi_coreb-install: libmcapi_coreb-build
	$(MAKE1) -C $(ICC_DIR)/libmcapi_coreb ICC_MACHINE=$(ICC_MACHINE) ICC_BUILDROOT=$(ICC_BUILDROOT) \
			DESTDIR=$(STAGING_DIR) install

icc_task-build:
	$(MAKE1) -C $(ICC_DIR)/example/task  KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE) \
			ICC_INCLUDE=$(ICC_INCLUDE) \
			MCAPI_INCLUDE=$(MCAPI_INCLUDE) \
			MCAPI_TEST_INCLUDE=$(MCAPI_TEST_INCLUDE) \
			LIBMCAPI_COREB=$(LIBMCAPI_COREB)
	$(MAKE1) -C $(ICC_DIR)/example/test_app KERNEL_DIR=$(KERNEL_DIR) ICC_MACHINE=$(ICC_MACHINE) \
			ICC_INCLUDE=$(ICC_INCLUDE) \
			MCAPI_INCLUDE=$(MCAPI_INCLUDE) \
			MCAPI_TEST_INCLUDE=$(MCAPI_TEST_INCLUDE) \
			LIBMCAPI_COREB=$(LIBMCAPI_COREB)

icc_task-install: icc_task-build
	@echo "install $(ICC_DIR)/example/task/ -> $(TARGET_DIR)/bin/"
	@for x in $(INSTALL_TASK); do \
	        test -f $(ICC_DIR)/example/task/$$x && $(INSTALL) -m 0444 -D $(ICC_DIR)/example/task/$$x $(TARGET_DIR)/bin/$$x; \
        done
	@echo "install $(ICC_DIR)/example/test_app/ -> $(TARGET_DIR)/bin/"
	@for x in $(INSTALL_TEST_APP); do \
	        test -f $(ICC_DIR)/example/test_app/$$x && $(INSTALL) -m 0755 -D $(ICC_DIR)/example/test_app/$$x $(TARGET_DIR)/bin/$$x; \
        done

icc_task-uninstall:
	@for x in $(INSTALL_TASK); do \
	        rm -rf $(TARGET_DIR)/bin/$$x; \
        done
	@for x in $(INSTALL_TEST_APP); do \
	        rm -rf $(TARGET_DIR)/bin/$$x; \
        done

icc-build: icc_core-build icc_loader-build libmcapi_coreb-build

icc-install: libmcapi_coreb-install 


ifeq ($(BR2_PACKAGE_ICC),y)
TARGETS+=icc
endif

