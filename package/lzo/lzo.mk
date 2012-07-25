#############################################################
#
# lzo
#
#############################################################
LZO_VERSION = 2.06
LZO_SITE = http://www.oberhumer.com/opensource/lzo/download
LZO_INSTALL_STAGING = YES

ifneq ($(BR2_ABI_FLAT),y)
LZO_CONF_OPT = --enable-shared
endif

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
