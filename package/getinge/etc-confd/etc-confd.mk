
ETC_CONFD_SOURCE  =
ETC_CONFD_VERSION = 1.0

ETC_CONFD_DEPENDENCIES = host-mtd netcom-apps

define ETC_CONFD_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/conf.d/
	cp -a board/getinge/netcom/conf.d/* $(BINARIES_DIR)/conf.d/
	$(HOST_DIR)/usr/sbin/mkfs.jffs2 -n -p -e 128 \
		-o $(TARGET_DIR)/usr/share/conf.d/defaults.img \
		-d $(BINARIES_DIR)/conf.d/
	install -D -m 0644 board/getinge/netcom/conf.d/version \
		$(TARGET_DIR)/usr/share/conf.d/version
endef

$(eval $(call GENTARGETS,package/getinge,etc-confd))
