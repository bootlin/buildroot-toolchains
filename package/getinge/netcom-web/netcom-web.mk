
NETCOM_WEB_SOURCE =
NETCOM_WEB_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/packages/netcom/web/html/

define NETCOM_WEB_EXTRACT_CMDS
	cp -a $(NETCOM_WEB_SITE)/* $(@D)/
endef

define NETCOM_WEB_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/html/
	cp -a $(@D)/* $(TARGET_DIR)/usr/share/html/
	ln -sf /usr/bin/fwupgrade-cgi $(TARGET_DIR)/usr/share/html/cgi-bin/fwupgrade-cgi
	install -D -m 0755 $($(PKG)_DIR_PREFIX)/netcom-web/S98httpd $(TARGET_DIR)/etc/init.d/S98httpd
	install -D -m 0644 $($(PKG)_DIR_PREFIX)/netcom-web/httpd.conf $(TARGET_DIR)/etc/httpd.conf
	install -D -m 0644 $($(PKG)_DIR_PREFIX)/netcom-web/variables $(TARGET_DIR)/usr/share/release/variables
endef

$(eval $(call GENTARGETS,package/getinge,netcom-web))
