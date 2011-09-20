# Makefile for serial server (sersrvd) by Johan Adolfsson <johana@axis.com>
# $Id: Makefile,v 1.2 2003/12/18 23:46:51 gsiftar Exp $

#SUBDIRS = comp

PROGS     = sersrvd 
LIBS      = libnetutil.a
INCLUDES  = netutil.h

INCDIR    = $(prefix)/include

INSTDIR   = $(prefix)/bin/
INSTCFGDIR= $(prefix)/etc/
CONFFILE  = sersrvd.conf
CONFMODE  = 0644
INSTMODE  = 0755
INSTOWNER = root
INSTGROUP = root

LIBDIR    = $(prefix)/lib
LIBMODE   = 0644

INCLUDEDIR  = $(prefix)/include
INCLUDEMODE = 0644

RANLIB ?= ranlib
INSTALL ?= install

SRCS = sersrvd.c webctrl_fifo.c netutil.c serinit.c serutil.c listener.c \
       tnetopt.c sersrvd_config.c bfdutil.c rfc2217.c axcomp.c \
       authip.c authuser.c debug.c

SERSRVDOBJS = sersrvd.o webctrl_fifo.o serinit.o serutil.o listener.o \
              tnetopt.o sersrvd_config.o bfdutil.o rfc2217.o axcomp.o \
              authip.o authuser.o debug.o

LSRCS	  = netutil.c 

LOBJS     = netutil.o

#CXXFLAGS = -Wall -g -Wno-ctor-dtor-privacy -ansi -pipe -fcheck-new
#CCFLAGS = -Wall -g

CFLAGS   += -Wall -I$(INCDIR)
LDFLAGS  += -L.
LDLIBS   += -lnetutil -lcrypt

all:	$(LIBS) $(PROGS) 

test: rawtest telnettest
rawtest: sersrvd
	# Create a fifo acting as a loopback device.
	-mkfifo test_dev
	# Test raw mode
	./sersrvd -f testraw.conf &
	sleep 1
	tcptest -otestraw.out -L 30 -t100000 -b500 localhost 4001
	tcptest -otestraw.out -L 10 -t10000 -b50 localhost 4001
	tcptest -otestraw.out -L 10 -t50000 -b5000 localhost 4001
	killall -TERM sersrvd
	sleep 1

telnettest: sersrvd
	-mkfifo test_dev
	./sersrvd -f testtelnet.conf &
	sleep 1
	tcptest -otesttelnet.out -telnet localhost 4001
	killall -TERM sersrvd
	sleep 1
	hd testtelnet.out

cleantest:
	rm -f test_dev *.out output

sersrvd:  $(SERSRVDOBJS) $(LIBS)
	$(CC) $(LDFLAGS) $(SERSRVDOBJS) $(LDLIBS) -o $@

libnetutil.a:	$(LOBJS)
	$(AR) crv libnetutil.a $(LOBJS)
	$(RANLIB) libnetutil.a

#install: install_lib $(PROGS)
install: install_lib $(PROGS)
	@echo "## Installing sersrvd"
	$(INSTALL) -d $(INSTDIR) $(INSTCFGDIR)
	$(INSTALL) -m $(INSTMODE) $(PROGS) $(INSTDIR)
	@if [ ! -f $(INSTCFGDIR)/$(CONFFILE) ]; then \
	  echo "## Installing sersrvd.conf"; \
	  $(INSTALL) -m $(CONFMODE) sersrvd.conf $(INSTCFGDIR); fi

install_lib:	$(LIBS)
	@echo "## Installing libs - netutil"
	$(INSTALL) -d $(LIBDIR) $(INCLUDEDIR)
	$(INSTALL) -m $(LIBMODE) $(LIBS) $(LIBDIR)
	$(INSTALL) -m $(INCLUDEMODE) $(INCLUDES) $(INCLUDEDIR)

clean: cleantest
	rm -f $(PROGS) $(LIBS) *.o core

depend:
	makedepend -Y -- $(CFLAGS) -- $(SRCS) 2>/dev/null

# The following is used to automatically generate dependencies.
# DO NOT DELETE

sersrvd.o: sersrvd.h webctrl_fifo.h sersrvd_config.h serinit.h bfdutil.h
sersrvd.o: tnetopt.h debug.h netutil.h serutil.h rfc2217.h axcomp.h
sersrvd.o: listener.h authip.h authuser.h
webctrl_fifo.o: webctrl_fifo.h
netutil.o: netutil.h
serinit.o: serinit.h serutil.h
serutil.o: serutil.h
listener.o: listener.h bfdutil.h webctrl_fifo.h sersrvd_config.h serinit.h
listener.o: tnetopt.h debug.h netutil.h sersrvd.h
tnetopt.o: tnetopt.h debug.h netutil.h rfc2217.h axcomp.h
sersrvd_config.o: sersrvd_config.h serinit.h sersrvd.h webctrl_fifo.h
sersrvd_config.o: bfdutil.h tnetopt.h debug.h serutil.h listener.h netutil.h
bfdutil.o: bfdutil.h webctrl_fifo.h sersrvd_config.h serinit.h tnetopt.h
bfdutil.o: debug.h netutil.h
rfc2217.o: rfc2217.h tnetopt.h debug.h netutil.h bfdutil.h webctrl_fifo.h
rfc2217.o: sersrvd_config.h serinit.h serutil.h
axcomp.o: rfc2217.h tnetopt.h debug.h axcomp.h netutil.h bfdutil.h
axcomp.o: webctrl_fifo.h sersrvd_config.h serinit.h serutil.h
authip.o: authip.h
authuser.o: authuser.h
debug.o: debug.h
