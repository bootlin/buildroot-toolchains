#############################################################
#
# Xenomai
#
#############################################################
XENOMAI_VERSION = 2.5.5.2
XENOMAI_SERIES = 25
XENOMAI_SITE =http://download.gna.org/xenomai/stable
XENOMAI_SOURCE =xenomai-2.5.5.2.tar.bz2
XENOMAI_CONF_OPT =  --includedir=$(TOOLCHAIN_DIR)/arm-none-linux-gnueabi/libc/usr/include --host=arm-none-linux-gnueabi --enable-arm-mach=omap3 --enable-arm-tsc

define XENOMAI_PATCH_KERNEL
         output/build/xenomai-2.5.5.2/scripts/prepare-kernel.sh --linux=linux/linux-2.6.x --adeos=output/build/xenomai-2.5.5.2/ksrc/arch/arm/patches/adeos-ipipe-2.6.33-arm-1.18-00.patch --arch=arm

endef

XENOMAI_POST_PATCH_HOOKS += XENOMAI_PATCH_KERNEL

$(eval $(call AUTOTARGETS,package,xenomai))
