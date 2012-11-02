#############################################################
#
# gnuplot old version for uclinux
#
#############################################################
GNUPLOT_OLD_SITE = svn://firewall-sources.blackfin.uclinux.org:80/svn/uclinux-dist/trunk/user/gnuplot
GNUPLOT_OLD_SITE_METHOD = svn
GNUPLOT_OLD_VERSION = HEAD
GNUPLOT_OLD_DEPENDENCIES = libpng zlib

GNUPLOT_OLD_EXTRA_CFLAGS = -I. -DSHORT_TERMLIST -DHAVE_GETCWD

define GNUPLOT_OLD_CONFIGURE_TERM
        $(TARGET_CC) $(TARGET_LDFLAGS) $(GNUPLOT_OLD_EXTRA_CFLAGS) -I$(@D)/term -I$(STAGING_DIR)/usr/include \
			-I$(STAGING_DIR)/usr/include \
	                       -c $(@D)/term.c
endef

GNUPLOT_OLD_POST_CONFIGURE_HOOKS += GNUPLOT_OLD_CONFIGURE_TERM

define GNUPLOT_OLD_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D) 
endef

define GNUPLOT_OLD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/gnuplot $(TARGET_DIR)/bin/
endef

define GNUPLOT_OLD_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define GNUPLOT_OLD_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/bin/$(GNUPLOT_OLD_EXE)
endef

$(eval $(generic-package))
