#############################################################
#
# bfin gptimer test
#
#############################################################
BFIN_GPTIMER_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/blkfin-test/gptimer_test
BFIN_GPTIMER_SITE_METHOD = svn
BFIN_GPTIMER_VERSION = HEAD

TARGET_CFLAGS+=-I$(LINUX26_SOURCE_DIR)/include -I$(LINUX26_SOURCE_DIR)/arch/blackfin/include

BFIN_GPTIMER_SRC:=simple_timer_test.c
BFIN_GPTIMER_EXE:=simple_timer_test

define BFIN_GPTIMER_BUILD_CMDS
        $(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
	               $(@D)/$(BFIN_GPTIMER_SRC) -o $(@D)/$(BFIN_GPTIMER_EXE)
endef

define BFIN_GPTIMER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(BFIN_GPTIMER_EXE) $(TARGET_DIR)/bin/
endef

define BFIN_GPTIMER_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define BFIN_GPTIMER_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/$(BFIN_GPTIMER_EXE)
endef

$(eval $(call GENTARGETS,package,bfin_gptimer))
