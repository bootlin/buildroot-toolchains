
NETCOM_WEB_SITE = git@github.com:gsiftar/NetCOM-web.git
NETCOM_WEB_SITE_METHOD = git
NETCOM_WEB_VERSION = master

define NETCOM_WEB_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/html/
	cp -a $(@D)/* $(TARGET_DIR)/usr/share/html/
	ln -sf /usr/bin/fwupgrade-cgi $(TARGET_DIR)/usr/share/html/cgi-bin/fwupgrade-cgi
	install -D -m 0755 $($(PKG)_DIR_PREFIX)/netcom-web/S98httpd $(TARGET_DIR)/etc/init.d/S98httpd
	install -D -m 0644 $($(PKG)_DIR_PREFIX)/netcom-web/httpd.conf $(TARGET_DIR)/etc/httpd.conf
	install -D -m 0755 $($(PKG)_DIR_PREFIX)/netcom-web/nclic $(TARGET_DIR)/bin/nclic
	# Needed for compatibility with the Axis device, on which
	# haserl is installed in /bin/haserl.
	(cd $(TARGET_DIR) ; ln -sf ../usr/bin/haserl bin/haserl)
endef

$(eval $(call GENTARGETS,package/getinge,netcom-web))
