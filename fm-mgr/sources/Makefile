SRCS = fm_main.cpp
OBJS = fm_main.o

OBJS = $(SRCS:.cpp=.o)
INCLUDES = -I.
CCFLAGS = -g -O2 -Wall -Werror
EXTRACCFLAGS = -Wformat -Wformat-security

MAJOR ?= 1
MINOR ?= 0

DESTDIR ?= /
BINDIR ?= /usr/local/bin
SYSCONFDIR ?= /etc
UNITDIR ?= /usr/lib/systemd/system


build: fmManager

.cpp.o:
	$(CXX) $(INCLUDES) $(CCFLAGS)  $(EXTRACCFLAGS) -c $< -o $@

fmManager: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) -lfmcommon -lrt -lpthread -luuid

clean:
	 @rm -f $(OBJS)

install:
	mkdir -p $(DESTDIR)/usr/lib/systemd/system
	install -m 755 -d $(DESTDIR)$(SYSCONFDIR)
	install -m 755 -d $(DESTDIR)$(SYSCONFDIR)/init.d
	install -m 755 -d $(DESTDIR)$(SYSCONFDIR)/logrotate.d
	install -m 755 -d $(DESTDIR)$(BINDIR)
	install -m 755 fminit $(DESTDIR)$(SYSCONFDIR)/init.d/fminit
	install -m 755 fmManager $(DESTDIR)$(BINDIR)/fmManager
	install -m 644 fm.logrotate $(DESTDIR)$(SYSCONFDIR)/logrotate.d/fm.logrotate
	install -m 644 fminit.service $(DESTDIR)$(UNITDIR)/fminit.service
