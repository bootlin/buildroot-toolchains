#############################################################
#
# gnuplot
#
#############################################################
GNUPLOT_VERSION = 4.6.0
GNUPLOT_SOURCE = gnuplot-$(GNUPLOT_VERSION).tar.gz
GNUPLOT_SITE = http://superb-dca2.dl.sourceforge.net/project/gnuplot/gnuplot/$(GNUPLOT_VERSION)
GNUPLOT_INSTALL_STAGING = YES
GNUPLOT_INSTALL_TARGET = YES
GNUPLOT_CONF_OPT = --without-demo --disable-demo

$(eval $(autotools-package))
