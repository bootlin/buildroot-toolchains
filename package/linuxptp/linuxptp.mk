#############################################################
#
# linuxptp
#
#############################################################
LINUXPTP_VERSION = HEAD
LINUXPTP_SITE = http://git.code.sf.net/p/linuxptp/code
LINUXPTP_SITE_METHOD = git

define LINUXPTP_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" -C $(@D)
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -o $(TOPDIR)/linux/linux-kernel/Documentation/ptp/testptp \
		-lrt $(TOPDIR)/linux/linux-kernel/Documentation/ptp/testptp.c
endef

define LINUXPTP_INSTALL_TARGET_CMDS
	cp -dpf $(@D)/ptp4l $(@D)/pmc $(@D)/phc2sys $(@D)/hwstamp_ctl $(TARGET_DIR)/usr/sbin
	cp -dpf $(TOPDIR)/linux/linux-kernel/Documentation/ptp/testptp $(TARGET_DIR)/usr/sbin
endef

$(eval $(generic-package))
