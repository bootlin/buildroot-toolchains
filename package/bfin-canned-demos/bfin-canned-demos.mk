#############################################################
#
# bfin canned demos
#
#############################################################
BFIN_CANNED_DEMOS_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-apps/ndso
BFIN_CANNED_DEMOS_SITE_METHOD = svn
BFIN_CANNED_DEMOS_VERSION = HEAD

BFIN_CANNED_DEMOS_TARGET =
BFIN_CANNED_DEMOS_INSTALL =

ifeq ($(BR2_PACKAGE_USER_NDSO),y)
BFIN_CANNED_DEMOS_TARGET+=src_ndso
define BR2_PACKAGE_USER_NDSO_INSTALL
rm -rf $(TARGET_DIR)/home/httpd
cp -rdpf $(@D)/web_ndso  $(TARGET_DIR)/home/httpd
$(INSTALL) -m 0755 -D $(@D)/src_ndso/ndso  $(TARGET_DIR)/home/httpd/cgi-bin/ndso.cgi
endef
BFIN_CANNED_DEMOS_INSTALL+=BR2_PACKAGE_USER_NDSO_INSTALL
endif

ifeq ($(BR2_PACKAGE_USER_NDSO_IIO),y)
BFIN_CANNED_DEMOS_TARGET+=src_ndso_iio
define BR2_PACKAGE_USER_NDSO_IIO_INSTALL
rm -rf $(TARGET_DIR)/home/httpd
cp -rdpf $(@D)/web_ndso_iio  $(TARGET_DIR)/home/httpd
$(INSTALL) -m 0755 -D $(@D)/src_ndso_iio/ndso  $(TARGET_DIR)/home/httpd/cgi-bin/ndso.cgi
endef
BFIN_CANNED_DEMOS_INSTALL+=BR2_PACKAGE_USER_NDSO_IIO_INSTALL
endif

ifeq ($(BR2_PACKAGE_USER_AWG),y)
BFIN_CANNED_DEMOS_TARGET+=src_awg
define BR2_PACKAGE_USER_AWG_INSTALL
rm -rf $(TARGET_DIR)/home/httpd
cp -rdpf $(@D)/web_awg  $(TARGET_DIR)/home/httpd
$(INSTALL) -m 0755 -D $(@D)/src_awg/awg  $(TARGET_DIR)/home/httpd/cgi-bin/awg.cgi
endef
BFIN_CANNED_DEMOS_INSTALL+=BR2_PACKAGE_USER_AWG_INSTALL
endif

ifeq ($(BR2_PACKAGE_USER_PPIFPGACGI),y)
BFIN_CANNED_DEMOS_TARGET+=src_ppi_fpga_prj
define BR2_PACKAGE_USER_PPIFPGACGI_INSTALL
rm -rf $(TARGET_DIR)/home/httpd
cp -rdpf $(@D)/web_ppi_fpga_prj $(TARGET_DIR)/home/httpd
$(INSTALL) -m 0755 -D $(@D)/src_ppi_fpga_prj/fpga  $(TARGET_DIR)/home/httpd/cgi-bin/fpga.cgi
endef
BFIN_CANNED_DEMOS_INSTALL+=BR2_PACKAGE_USER_PPIFPGACGI_INSTALL
endif

define BFIN_CANNED_DEMOS_BUILD_CMDS
	for demo_targets in $(BFIN_CANNED_DEMOS_TARGET); do \
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/$$demo_targets; \
	done
endef

define BFIN_CANNED_DEMOS_INSTALL_TARGET_CMDS
if ! [ -d $(TARGET_DIR)/home ]; then \
	mkdir -p $(TARGET_DIR)/home; \
fi
$(foreach package_install, $(BFIN_CANNED_DEMOS_INSTALL), $(call $(package_install)$(sep)))
if [ -f $(TARGET_DIR)/home/httpd/index.html ]; then \
	ln -sf index.htm $(TARGET_DIR)/home/httpd/index.html; \
fi
endef

define BFIN_CANNED_DEMOS_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define BFIN_CANNED_DEMOS_UNINSTALL_TARGET_CMDS
	rm -rf $(TARGET_DIR)/home/httpd
endef

$(eval $(generic-package))
