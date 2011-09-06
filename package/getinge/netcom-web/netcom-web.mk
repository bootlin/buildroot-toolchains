NETCOM_WEB_SOURCE =
NETCOM_WEB_VERSION = 0.1

define NETCOM_WEB_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/html/
	cp -a $($(PKG)_DIR_PREFIX)/netcom-web/src/* $(TARGET_DIR)/usr/share/html/
	ln -sf /usr/bin/fwupgrade-cgi $(TARGET_DIR)/usr/share/html/cgi-bin/fwupgrade-cgi
	install -D -m 0755 $($(PKG)_DIR_PREFIX)/netcom-web/S98httpd $(TARGET_DIR)/etc/init.d/S98httpd
endef

$(eval $(call GENTARGETS,package/getinge,netcom-web))
