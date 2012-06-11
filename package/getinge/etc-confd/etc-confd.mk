
ETC_CONFD_SOURCE  =
ETC_CONFD_VERSION = 1.0

# We need mkfs.jffs2
ETC_CONFD_DEPENDENCIES = host-mtd

# These are listed as dependencies because they install files in in
# $(BINARIES_DIR)/conf.d/, so their installation procedure must be
# executed before ours.

ETC_CONFD_DEPENDENCIES += netcom-apps netcom-web

define ETC_CONFD_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/conf.d/
	cp -a board/getinge/netcom/conf.d/* $(BINARIES_DIR)/conf.d/
	ln -sf /usr/share/zoneinfo/UTC $(BINARIES_DIR)/conf.d/localtime
	$(HOST_DIR)/usr/sbin/mkfs.jffs2 -n -p -e 128 \
		-o $(TARGET_DIR)/usr/share/conf.d/defaults.img \
		-d $(BINARIES_DIR)/conf.d/
	install -D -m 0644 board/getinge/netcom/conf.d/version \
		$(TARGET_DIR)/usr/share/conf.d/version
endef

$(eval $(call GENTARGETS,package/getinge,etc-confd))
