#############################################################
#
# Netembryo (libraries needed by Libnemesi)
#
#############################################################
NETEMBRYO_VERSION = 0.1.1
NETEMBRYO_SITE = http://lscube.org/files/downloads/netembryo
NETEMBRYO_SOURCE = netembryo-$(NETEMBRYO_VERSION).tar.bz2
NETEMBRYO_INSTALL_STAGING = YES

$(eval $(call AUTOTARGETS))
