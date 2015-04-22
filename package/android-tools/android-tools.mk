################################################################################
#
# android-tools
#
################################################################################

ANDROID_TOOLS_SITE = http://snapshot.debian.org/archive/debian/20141125T040054Z/pool/main/a/android-tools/
ANDROID_TOOLS_VERSION = 4.2.2+git20130529
ANDROID_TOOLS_SOURCE = android-tools_$(ANDROID_TOOLS_VERSION).orig.tar.xz
HOST_ANDROID_TOOLS_EXTRA_DOWNLOADS = android-tools_$(ANDROID_TOOLS_VERSION)-5.1.debian.tar.xz
ANDROID_TOOLS_LICENSE = Apache-2.0
ANDROID_TOOLS_LICENSE_FILES = debian/copyright

# Extract the Debian tarball inside the sources
define HOST_ANDROID_TOOLS_DEBIAN_EXTRACT
	$(call suitable-extractor,$(notdir $(HOST_ANDROID_TOOLS_EXTRA_DOWNLOADS))) \
		$(DL_DIR)/$(notdir $(HOST_ANDROID_TOOLS_EXTRA_DOWNLOADS)) | \
		$(TAR) -C $(@D) $(TAR_OPTIONS) -
endef

HOST_ANDROID_TOOLS_POST_EXTRACT_HOOKS += HOST_ANDROID_TOOLS_DEBIAN_EXTRACT

# Apply the Debian patches before applying the Buildroot patches
define HOST_ANDROID_TOOLS_DEBIAN_PATCH
	$(APPLY_PATCHES) $(@D) $(@D)/debian/patches \*
endef

HOST_ANDROID_TOOLS_PRE_PATCH_HOOKS += HOST_ANDROID_TOOLS_DEBIAN_PATCH

ifeq ($(BR2_PACKAGE_HOST_ANDROID_TOOLS_FASTBOOT),y)
HOST_ANDROID_TOOLS_TARGETS += fastboot
HOST_ANDROID_TOOLS_DEPENDENCIES += host-zlib host-libselinux
endif

ifeq ($(BR2_PACKAGE_HOST_ANDROID_TOOLS_ADB),y)
HOST_ANDROID_TOOLS_TARGETS += adb
HOST_ANDROID_TOOLS_DEPENDENCIES += host-zlib host-libselinux host-openssl
endif

define HOST_ANDROID_TOOLS_BUILD_CMDS
	$(foreach t,$(HOST_ANDROID_TOOLS_TARGETS),\
		mkdir -p $(@D)/build-$(t) && \
		$(HOST_MAKE_ENV) $(HOST_CONFIGURE_OPTS) $(MAKE) SRCDIR=$(@D) \
			-C $(@D)/build-$(t) -f $(@D)/debian/makefiles/$(t).mk$(sep))
endef

define HOST_ANDROID_TOOLS_INSTALL_CMDS
	$(foreach t,$(HOST_ANDROID_TOOLS_TARGETS),\
		$(INSTALL) -D -m 0755 $(@D)/build-$(t)/$(t) $(HOST_DIR)/usr/bin/$(t)$(sep))
endef

$(eval $(host-generic-package))
