#############################################################
#
# V4L2 video test application
#
#############################################################
V4L2_VIDEO_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/v4l2_video_test
V4L2_VIDEO_TEST_SITE_METHOD = svn
V4L2_VIDEO_TEST_VERSION = HEAD

define V4L2_VIDEO_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define V4L2_VIDEO_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	$(INSTALL) -D -m 0755 $(@D)/v4l2_video_test $(TARGET_DIR)/bin/
endef

define V4L2_VIDEO_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define V4L2_VIDEO_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/v4l2_video_test
endef

$(eval $(call GENTARGETS,package,v4l2_video_test))
