#
# SPDX-License-Identifier: Apache-2.0
#

MAJOR ?= 1
MINOR ?= 0
PATCH ?= 0
MIBVER ?= 2.0

DEST_DIR ?= /
DATA_DIR ?= /usr/share
LIB_DIR ?= /usr/lib64
SDK_DEPLOY_DIR ?= /opt/deploy/cgcs_sdk

SRCS = cgtsActiveAlarms.c cgtsAgentPlugin.c cgtsEventLogs.c

OBJS = $(SRCS:.c=.o)
LDLIBS = -lfmcommon -lnetsnmp -lnetsnmpagent -luuid
INCLUDES += -I./
CCFLAGS = -fPIC -g -O2 -Wall -Werror -Wformat -Wformat-security
LIBCGTSAGENT_SO := libcgtsAgentPlugin.so
MIBDIR = wrs-snmp-mib-$(MAJOR).$(MIBVER)

build: lib
	tar -zc --transform=s/^mibs/$(MIBDIR)/ -f wrs-snmp-mib-$(MAJOR).$(MIBVER).tgz mibs

.c.o:
	$(CXX) $(CCFLAGS) $(INCLUDES) -c $< -o $@

lib: $(OBJS)
	$(CXX) $(OBJS) $(LDLIBS) -o $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) -shared
	ln -sf $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(LIBCGTSAGENT_SO).$(MAJOR)
	ln -sf $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(LIBCGTSAGENT_SO)

clean:
	@rm -f $(OBJ) *.o *.so.* *.tgz

install:
	install -m 755 -d $(DEST_DIR)$(LIB_DIR)
	install -m 755 $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DEST_DIR)$(LIB_DIR)
	install -m 755 -d $(DEST_DIR)$(DATA_DIR)/snmp/mibs
	install -m 644 mibs/wrsAlarmMib.mib.txt $(DEST_DIR)$(DATA_DIR)/snmp/mibs/
	install -m 644 mibs/wrsEnterpriseReg.mib.txt $(DEST_DIR)$(DATA_DIR)/snmp/mibs/
	install -m 755 -d $(SDK_DEPLOY_DIR)
	install -m 644 wrs-snmp-mib-$(MAJOR).$(MIBVER).tgz $(SDK_DEPLOY_DIR)/
	ln -s $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DEST_DIR)$(LIB_DIR)/$(LIBCGTSAGENT_SO).$(MAJOR)
	ln -s $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DEST_DIR)$(LIB_DIR)/$(LIBCGTSAGENT_SO)
