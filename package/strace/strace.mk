#############################################################
#
# strace
#
#############################################################

STRACE_VERSION = 10736
STRACE_SITE_METHOD = svn
STRACE_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/strace

FLTFLAGS += -s 0x5000

STRACE_CONF_ENV = ac_cv_header_linux_if_packet_h=yes \
		  ac_cv_header_linux_netlink_h=yes \
	          ac_cv_type_stat64=no \
		  ac_cv_have_long_long_off_t=no

STRACE_SUBDIR = strace-4.6

define STRACE_CHECK_ENTITIES 
	cd $(@D) && \
	./check-entities.sh $(TOPDIR)/$(LINUX26_SOURCE_DIR) $(KERNEL_ARCH) $(STRACE_SUBDIR) $(ARCH)
endef
STRACE_PRE_CONFIGURE_HOOKS += STRACE_CHECK_ENTITIES


define STRACE_REMOVE_STRACE_GRAPH
	rm -f $(TARGET_DIR)/usr/bin/strace-graph
endef

STRACE_POST_INSTALL_TARGET_HOOKS += STRACE_REMOVE_STRACE_GRAPH

$(eval $(call AUTOTARGETS,package,strace))
