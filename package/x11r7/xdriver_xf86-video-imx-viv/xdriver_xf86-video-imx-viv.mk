################################################################################
#
# xdriver_xf86-video-imx-viv
#
################################################################################

XDRIVER_XF86_VIDEO_IMX_VIV_VERSION = $(FREESCALE_IMX_VERSION)
XDRIVER_XF86_VIDEO_IMX_VIV_SITE = $(FREESCALE_IMX_SITE)
XDRIVER_XF86_VIDEO_IMX_VIV_SOURCE = xserver-xorg-video-imx-viv-$(XDRIVER_XF86_VIDEO_IMX_VIV_VERSION).tar.gz
XDRIVER_XF86_VIDEO_IMX_VIV_DEPENDENCIES = gpu-viv-bin-mx6q xserver_xorg-server \
	xproto_xproto xproto_xf86driproto
XDRIVER_XF86_VIDEO_IMX_VIV_INSTALL_STAGING = YES
XDRIVER_XF86_VIDEO_IMX_VIV_MAKE_CMD = $(TARGET_CONFIGURE_OPTS) $(MAKE1) -C $(@D)/EXA/src -f makefile.linux

define XDRIVER_XF86_VIDEO_IMX_VIV_BUILD_CMDS
	$(XDRIVER_XF86_VIDEO_IMX_VIV_MAKE_CMD) sysroot=$(STAGING_DIR) \
		BUSID_HAS_NUMBER=1 BUILD_IN_YOCTO=1 XSERVER_GREATER_THAN_13=1 \
		CFLAGS+="$(TARGET_CFLAGS) -I$(@D)/DRI_1.10.4/src"
endef

define XDRIVER_XF86_VIDEO_IMX_VIV_INSTALL_STAGING_CMDS
	$(XDRIVER_XF86_VIDEO_IMX_VIV_MAKE_CMD) prefix=$(STAGING_DIR)/usr install
	$(INSTALL) -m 644 $(@D)/EXA/src/vivante_gal/vivante_priv.h $(STAGING_DIR)/usr/include
	$(INSTALL) -m 644 $(@D)/EXA/src/vivante_gal/vivante_gal.h $(STAGING_DIR)/usr/include
endef

define XDRIVER_XF86_VIDEO_IMX_VIV_INSTALL_TARGET_CMDS
	$(XDRIVER_XF86_VIDEO_IMX_VIV_MAKE_CMD) prefix=$(TARGET_DIR)/usr install
endef

$(eval $(generic-package))

