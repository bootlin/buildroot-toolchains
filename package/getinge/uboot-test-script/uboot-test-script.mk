
UBOOT_TEST_SCRIPT_SOURCE =
UBOOT_TEST_SCRIPT_DEPENDENCIES = host-uboot-tools

# first strip all comments and blank lines
define UBOOT_TEST_SCRIPT_BUILD_CMDS
  sed -e 's/#.*//' -e 's/[ ^I]*$$//' -e '/^$$/ d' package/getinge/uboot-test-script/src/uboot.script > $(@D)/uboot.script.clean
	$(HOST_DIR)/usr/bin/mkimage 				\
		-T script 					\
		-C none 					\
		-n 'Hardware Test Script File' 				\
		-d $(@D)/uboot.script.clean \
		$(@D)/uboot.script.img
endef

# We're cheating here since we don't install things to the target
# directory. Buildroot has been improved a little bit about this in
# more recent versions (addition of <pkg>_INSTALL_IMAGES_CMDS), but
# it's not a major problem to do this hack.
define UBOOT_TEST_SCRIPT_INSTALL_TARGET_CMDS
  cp $(@D)/uboot.script.img $(BINARIES_DIR)/uboot.script.img
endef

$(eval $(call GENTARGETS,package/getinge,uboot-test-script))
