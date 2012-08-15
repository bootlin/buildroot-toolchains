#############################################################
#
# bfin linkport test
#
#############################################################
BFIN_LINKPORT_SOURCE =

BFIN_LINKPORT_SRC:=linkport_test.c
BFIN_LINKPORT_EXE:=linkport_test

define BFIN_LINKPORT_BUILD_CMDS
        $(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
	      $(TOPDIR)/package/bfin-linkport/$(BFIN_LINKPORT_SRC) -o $(@D)/$(BFIN_LINKPORT_EXE)
endef

define BFIN_LINKPORT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(BFIN_LINKPORT_EXE) $(TARGET_DIR)/bin/
endef

define BFIN_LINKPORT_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define BFIN_LINKPORT_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/$(BFIN_LINKPORT_EXE)
endef

$(eval $(call GENTARGETS))
