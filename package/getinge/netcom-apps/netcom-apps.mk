NETCOM_APPS_SOURCE =
NETCOM_APPS_SITE = $(GETINGE_APPS_TOPDIR)

NETCOM_APPS_DEPENDENCIES = netcom-cgic

define NETCOM_APPS_EXTRACT_CMDS
	cp -a $(NETCOM_APPS_SITE)/* $(@D)/
endef

# List of apps to install
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_AGS),AGS)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_DATABUFFERD),databufferd)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_DEVCOMD),devcomd)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_MODBUSD),modbusd)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_NCLIC),nclic)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_NCUTILD),ncutild)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_NETCOMLOCATOR),netcomlocator)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_PACS2000D),pacs2000d)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_PACSBUFFERD),pacsbufferd)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_PRINTLOGD),printlogd)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_PRINTSCAND),printscand)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_PRTUTIL),prtutil)
# Disabled, needs AeOSLocal.h, origin unknown
# NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_REMOTEAGENT),remoteAgent)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_SERSRVD),sersrvd)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_SNTPDATE),sntpdate)
NETCOM_APPS += $(if $(BR2_PACKAGE_NETCOM_APPS_WEBSETUP),websetup)

# List of daemons that need init scripts
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_AGS),ags_mach_intf table_intf shuttle_intf ags_super ags_mach_intf2)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_DATABUFFERD),databufferd)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_DEVCOMD),devcomd)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_MODBUSD),modbusd)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_NCUTILD),ncutild)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_PACS2000D),pacs2000d)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_PACSBUFFERD),pacsbufferd)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_PRINTLOGD),printlogd)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_PRINTSCAND),printscand)
NETCOM_DAEMONS += $(if $(BR2_PACKAGE_NETCOM_APPS_SERSRVD),sersrvd)

NETCOM_APPS_BUILD_FLAGS = 		\
	CC="$(TARGET_CC)" 		\
	CXX="$(TARGET_CXX)" 		\
	LD="$(TARGET_LD)" 		\
	AR="$(TARGET_AR)"		\
	RANLIB="$(TARGET_RANLIB)"	\
	CFLAGS="$(TARGET_CFLAGS)" 	\
	CXXFLAGS="$(TARGET_CXXFLAGS)" 	\
	LDFLAGS="$(TARGET_LDFLAGS)"	\
	BUILDROOT=1

define NETCOM_APPS_BUILD_CMDS
	# We have a special case for sersrvd, that has a src/
	# subdirectory
	for app in $(NETCOM_APPS) ; do \
		$(MAKE) -C $(@D)/$$app $(NETCOM_APPS_BUILD_FLAGS) clean || exit 1 ; \
		$(MAKE) -C $(@D)/$$app $(NETCOM_APPS_BUILD_FLAGS) || exit 1 ; \
		if [ $$app = "sersrvd" ] ; then \
			$(MAKE) -C $(@D)/$$app/src $(NETCOM_APPS_BUILD_FLAGS) clean || exit 1 ; \
			$(MAKE) -C $(@D)/$$app/src $(NETCOM_APPS_BUILD_FLAGS) || exit 1 ; \
		fi ; \
	done
endef

NETCOM_APPS_INSTALL_FLAGS = 		\
	BUILDROOT=1			\
	DESTDIR="$(TARGET_DIR)"		\
	INSTALL="$(INSTALL)"

define NETCOM_APPS_INSTALL_TARGET_CMDS
	# We have a special case for sersrvd, that has a src/
	# subdirectory
	for app in $(NETCOM_APPS) ; do \
		$(MAKE) -C $(@D)/$$app $(NETCOM_APPS_INSTALL_FLAGS) install || exit 1 ; \
		if [ $$app = "sersrvd" ] ; then \
			$(MAKE) -C $(@D)/$$app/src $(NETCOM_APPS_INSTALL_FLAGS) install || exit 1 ; \
		fi ; \
	done
	# Put the configuration files in a separate directories, since
	# they belong to a different filesystem.
	mkdir -p $(BINARIES_DIR)/conf.d/
	mv $(TARGET_DIR)/etc/conf.d/* $(BINARIES_DIR)/conf.d/
	install -D -m 0755 package/getinge/netcom-apps/netcom-common \
		 $(TARGET_DIR)/etc/init.d/netcom-common
	for daemon in $(NETCOM_DAEMONS) ; do \
		ln -sf netcom-common $(TARGET_DIR)/etc/init.d/S80$$daemon ; \
	done
endef

$(eval $(call GENTARGETS,package/getinge,netcom-apps))
