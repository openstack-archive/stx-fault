SRCS = cgtsActiveAlarms.c cgtsAgentPlugin.c cgtsEventLogs.c

OBJS = $(SRCS:.c=.o)
LDLIBS = -lfmcommon -lnetsnmp -lnetsnmpagent -luuid
INCLUDES += -I./
CCFLAGS = -fPIC -g -O2 -Wall -Werror -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security
LDFLAGS = -Wl,-z,noexecstack,-z,relro,-z,now
LIBCGTSAGENT_SO := libcgtsAgentPlugin.so
build: lib 

.c.o:
	$(CXX) $(CCFLAGS) $(INCLUDES) -c $< -o $@

lib: $(OBJS)
	$(CXX) $(OBJS) $(LDLIBS) -o $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) -shared $(LDFLAGS)
	ln -sf $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(LIBCGTSAGENT_SO).$(MAJOR)
	ln -sf $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(LIBCGTSAGENT_SO)

clean:
	@rm -f $(OBJ) *.o *.so.* 

install_non_bb:
	install -m 755 -d $(DEST_DIR)$(LIB_DIR)
	install -m 755 $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DEST_DIR)$(LIB_DIR)
	ln -s $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DEST_DIR)$(LIB_DIR)/$(LIBCGTSAGENT_SO).$(MAJOR)
	ln -s $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DEST_DIR)$(LIB_DIR)/$(LIBCGTSAGENT_SO)
