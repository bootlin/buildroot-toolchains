
LIBAGENTEMBEDDED_SOURCE =
LIBAGENTEMBEDDED_SITE = $(BR2_PACKAGE_GETINGE_AXIS_SDK_PATH)/libs/libAgentEmbedded/
LIBAGENTEMBEDDED_INSTALL_STAGING = YES
LIBAGENTEMBEDDED_DEPENDENCIES = openssl expat zlib

# Only a static library, nothing to install on the target
LIBAGENTEMBEDDED_INSTALL_TARGET = NO

define LIBAGENTEMBEDDED_EXTRACT_CMDS
	cp -a $(LIBAGENTEMBEDDED_SITE)/* $(@D)/
endef

define LIBAGENTEMBEDDED_BUILD_CMDS
	$(MAKE) -C $(@D) CROSS_COMPILE="$(TARGET_CROSS)" BUILDROOT=1
endef

define LIBAGENTEMBEDDED_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/libAgentEmbedded.a $(STAGING_DIR)/usr/lib/libAgentEmbedded.a
	for i in $(@D)/Include/*.h ; do \
		$(INSTALL) -D -m 0644 $$i $(STAGING_DIR)/usr/include/AgentEmbedded/`basename $$i` ; \
	done
	$(INSTALL) -D -m 0644 $(@D)/Sysdeps/Unix/AeOSLocal.h $(STAGING_DIR)/usr/include/AgentEmbedded/AeOSLocal.h
endef

$(eval $(call GENTARGETS,package/getinge,libagentembedded))
