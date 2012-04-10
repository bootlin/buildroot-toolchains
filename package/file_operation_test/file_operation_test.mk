#############################################################
#
# file_operation test
#
#############################################################
FILE_OPERATION_TEST_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/file_operation-test
FILE_OPERATION_TEST_SITE_METHOD = svn
FILE_OPERATION_TEST_VERSION = HEAD

FILE_OPERATION_TEST_SRCS=$(wildcard $(@D)/*.c)
FILE_OPERATION_TEST_TARGETS=$(patsubst %.c,%,$(notdir $(FILE_OPERATION_TEST_SRCS)))

define FILE_OPERATION_TEST_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define FILE_OPERATION_TEST_INSTALL_TARGET_CMDS
	if ! [ -d "$(TARGET_DIR)/bin/" ]; then \
		mkdir -p $(TARGET_DIR)/bin/; \
	fi
	echo "Start to install: $(FILE_OPERATION_TEST_TARGETS) "
	for i in $(FILE_OPERATION_TEST_TARGETS); do $(INSTALL) -D -m 0755 $(@D)/$$i $(TARGET_DIR)/bin/ ; done
	$(INSTALL) -D -m 0755 $(@D)/fs_test.sh           $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/fs_inod/err_log      $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/fs_inod/fs_error_log $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/fs_inod/fs_inod_bf   $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/fs_inod/make_subdirs $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/fs_inod/rm_files     $(TARGET_DIR)/bin/
	$(INSTALL) -D -m 0755 $(@D)/fs_inod/touch_files  $(TARGET_DIR)/bin/

endef

define FILE_OPERATION_TEST_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define FILE_OPERATION_TEST_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/file_operation
endef

$(eval $(call GENTARGETS,package,file_operation_test))
