#############################################################
#
# bluez_utils
#
#############################################################
BLUEZ_UTILS_VERSION = 4.86
BLUEZ_UTILS_SOURCE = bluez-$(BLUEZ_UTILS_VERSION).tar.gz
BLUEZ_UTILS_SITE = http://www.kernel.org/pub/linux/bluetooth/
BLUEZ_UTILS_AUTORECONF = NO
BLUEZ_UTILS_INSTALL_STAGING = NO
BLUEZ_UTILS_INSTALL_TARGET = YES
BLUEZ_UTILS_DEPENDENCIES += dbus libglib2
BLUEZ_UTILS_CONF_OPT = \

$(eval $(call AUTOTARGETS,package,bluez_utils))
